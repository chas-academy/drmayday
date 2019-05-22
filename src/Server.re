open Express;

let app = express();

App.use(app, Middleware.json());
App.use(app, Middleware.urlencoded(~extended=false, ()));

/* Serve React-app? */
App.get(app, ~path="/") @@
Middleware.from((_next, _req) => Response.sendStatus(Ok));

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