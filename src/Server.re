open Express;
open NirenoReasonablyTyped;

let app = express();

App.use(
  app,
  ExpressSession.(
    session(
      options(
        ~secret="secret",
        ~resave=false,
        ~saveUninitialized=false,
        ~cookie=cookieOptions(~secure=false, ~maxAge=43200 * 3600, ()),
        (),
      ),
    )
  ),
);
App.use(app, Middleware.json());
App.use(app, Middleware.urlencoded(~extended=false, ()));

App.useOnPath(app, ~path="/api") @@
PromiseMiddleware.from(ApiRoutes.requireLogin);

let static_path =
  BsNode.(
    Node.Global.(Node.Path.join([|__dirname, "../../../", "web", "build"|]))
  );

App.useOnPath(
  app,
  ~path="/",
  {
    let options = Static.defaultOptions();
    Static.make("/", options) |> Static.asMiddleware;
  },
);

/* API & Auth */
App.get(app, ~path="/slack/auth") @@ ApiRoutes.slackAuth;

App.get(app, ~path="/api/me") @@ ApiRoutes.me;

/* Receives channel events */
App.post(app, ~path="/event") @@ Routes.event;

/* Receives actions eg. Interactive Messages (Buttons etc) */
App.post(app, ~path="/actions") @@ Routes.actions;

/* Receives the /mayday command */
App.post(app, ~path="/mayday") @@ Routes.mayday;

/* Serve React-app? */
App.get(app, ~path="/Index.js") @@
Middleware.from((_next, _req, res) =>
  BsNode.(
    Response.sendFile(
      Node.Global.(Node.Path.resolve([|static_path, "Index.js"|])),
      "",
      res,
    )
  )
);

App.get(app, ~path="*") @@
Middleware.from((_next, _req, res) =>
  BsNode.(
    Response.sendFile(
      Node.Global.(Node.Path.resolve([|static_path, "index.html"|])),
      "",
      res,
    )
  )
);

let port = 4000;
let onListen = e =>
  switch (e) {
  | exception (Js.Exn.Error(e)) =>
    Js.log(e);

    Node.Process.exit(1);

  | _ => Js.log("Listening at http://localhost:" ++ port->string_of_int)
  };

App.listen(app, ~port, ~onListen, ());
