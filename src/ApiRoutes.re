open Express;
open NirenoReasonablyTyped;

let requireLogin = (next, req, res) => {
  let isLoggedIn =
    switch (ExpressSession.get(req, "user")) {
    | Some(_u) => true
    | None => false
    };

  let reqJson = Request.asJsonObject(req);

  let headers =
    switch (Js.Dict.get(reqJson, "headers")) {
    | Some(h) => h
    | None => Js_json.object_(Js.Dict.empty())
    };
  let token = Json.Decode.(headers |> field("authorization", string));

  Js.Promise.(
    Slack.IO.authTest(ServerHelpers.getBearerToken(token))
    |> then_(response => {
         let isLoggedIn =
           switch (Js_json.decodeBoolean(response##ok)) {
           | Some(o) => o
           | None => false
           };
         Js.log2("IsLoggedIn:", isLoggedIn);
         resolve(isLoggedIn);
       })
    |> then_(isLoggedIn =>
         isLoggedIn
           ? resolve(next(Next.route, res))
           : resolve(Response.sendStatus(Unauthorized, res))
       )
  );
};

let slackAuth =
  PromiseMiddleware.from((_next, req, res) => {
    let query = Request.query(req);

    switch (ServerHelpers.getDictString(query, "code")) {
    | Some(c) =>
      Js.Promise.(
        Slack.IO.makeAuthCallback(c)
        |> then_(response => {
             ExpressSession.set(req, "user", response##data##user_id);

             resolve(
               Response.sendJson(
                 Json.Encode.object_([
                   ("token", Js.Json.string(response##data##access_token)),
                 ]),
                 res,
               ),
             );
           })
      )
    | None => Js.Promise.resolve(Response.sendStatus(BadRequest, res))
    };
  });

let me =
  Middleware.from((_next, req) => {
    let user =
      switch (ExpressSession.get(req, "user")) {
      | Some(u) => u
      | None => ""
      };

    Response.sendJson(
      Json.Encode.object_([("user", Js.Json.string(user))]),
    );
  });

let logout =
  Middleware.from((_next, req) => {
    ExpressSession.set(req, "user", None);
    Response.sendStatus(Ok);
  });
