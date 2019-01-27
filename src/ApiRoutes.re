open Express;
open NirenoReasonablyTyped;

let requireLogin = (next, req, res) => {
  let isLoggedIn =
    switch (ExpressSession.get(req, "user")) {
    | Some(_u) => true
    | None => false
    };

  isLoggedIn ?
    next(Next.route, res) : Response.sendStatus(Unauthorized, res);
};

let slackAuth =
  PromiseMiddleware.from((_next, req, res) => {
    let query = Request.query(req);

    switch (ServerHelpers.getDictString(query, "code")) {
    | Some(c) =>
      Js.Promise.(
        Slack.IO.makeAuthCallback(c)
        |> then_(response => {
             ExpressSession.set(req, "user", response##data##user##id);

             resolve(Response.sendStatus(Ok, res));
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