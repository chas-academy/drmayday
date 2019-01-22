open Express;

let requireLogin = (next, req, res) => {
  let isLoggedIn =
    switch (Js.Dict.get(Request.asJsonObject(req), "session")) {
    | None => false
    | Some(s) =>
      switch (Js.Json.decodeObject(s)) {
      | Some(json) =>
        switch (Js.Dict.get(json, "user")) {
        | Some(_user) => true
        | _ => false
        }
      | _ => false
      }
    };

  isLoggedIn ?
    next(Next.route, res) : Response.sendStatus(Unauthorized, res);
};

let slackAuth =
  PromiseMiddleware.from((_next, req, res) => {
    let query = Request.query(req);
    let reqJson = Request.asJsonObject(req);

    switch (ServerHelpers.getDictString(query, "code")) {
    | Some(c) =>
      Js.Promise.(
        Slack.IO.makeAuthCallback(c)
        |> then_(response => {
             Js.Dict.set(
               reqJson,
               "session",
               ServerHelpers.makeUserObject(response##data##user##id),
             );

             resolve(Response.sendStatus(Ok, res));
           })
      )
    | None => Js.Promise.resolve(Response.sendStatus(BadRequest, res))
    };
  });

let me =
  Middleware.from((_next, req) => {
    Js.log2("Logged in:", Request.session(req));
    let reqJson = Request.asJsonObject(req);

    let user =
      Json.Decode.(
        switch (Js.Dict.get(reqJson, "session")) {
        | Some(s) => s |> field("user", string)
        | None => ""
        }
      );

    Response.sendJson(
      Json.Encode.object_([("user", Js.Json.string(user))]),
    );
  });

let logout =
  Middleware.from((_next, req) => {
    Js.Dict.set(Request.asJsonObject(req), "session", Js.Json.null);
    Response.sendStatus(Ok);
  });