open Express;

let app = express();

App.use(
  app,
  CookieSession.make(
    ~name="drmayday",
    ~keys=["key1", "key2"],
    ~secret="somesecret",
  ),
);
App.use(app, Middleware.json());
App.use(app, Middleware.urlencoded(~extended=false, ()));

App.useOnPath(app, ~path="/api") @@ Middleware.from(ApiRoutes.requireLogin);

/* Serve React-app? */
App.get(app, ~path="/") @@
Middleware.from((_next, _req) =>
  Response.sendString(
    {js|
  <a
    href="https://slack.com/oauth/authorize?scope=identity.basic,identity.email,identity.team,identity.avatar&client_id=166988133201.472402506917"
  >
    <img
      alt="Sign in with Slack"
      height="40"
      width="172"
      src="https://platform.slack-edge.com/img/sign_in_with_slack.png"
      srcset="https://platform.slack-edge.com/img/sign_in_with_slack.png 1x,
      https://platform.slack-edge.com/img/sign_in_with_slack@2x.png 2x"
      />
  </a>
|js},
  )
);

/* API & Auth */
App.get(app, ~path="/slack/auth") @@ ApiRoutes.slackAuth;

App.get(app, ~path="/api/me") @@ ApiRoutes.me;
App.get(app, ~path="/api/logout") @@ ApiRoutes.logout;

/* Receives channel events */
App.post(app, ~path="/event") @@ Routes.event;

/* Receives actions eg. Interactive Messages (Buttons etc) */
App.post(app, ~path="/actions") @@ Routes.actions;

/* Receives the /mayday command */
App.post(app, ~path="/mayday") @@ Routes.mayday;

let port = 4000;
let onListen = e =>
  switch (e) {
  | exception (Js.Exn.Error(e)) =>
    Js.log(e);
    Node.Process.exit(1);
  | _ => Js.log("Listening at http://localhost:" ++ port->string_of_int)
  };

App.listen(app, ~port, ~onListen, ());