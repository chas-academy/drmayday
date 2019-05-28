open Express;

let event =
  Middleware.from((_next, req, res) =>
    res
    |> (
      switch (Request.bodyJSON(req)) {
      | Some(body) =>
        let {eventType, event}: Slack.Event.eventPayload =
          Slack.Event.decodeEventPayload(body);

        switch (eventType) {
        | UrlVerification =>
          body |> Slack.Verification.challengeToken |> Response.sendString
        | EventCallback =>
          switch (event) {
          | Some(e) =>
            Event.handleEvent(e);
            Response.sendRawStatus(200);
          | None => Response.sendStatus(BadRequest)
          }

        | _ => Response.sendStatus(BadRequest)
        };
      | None => Response.sendStatus(BadRequest)
      }
    )
  );

let actions =
  Middleware.from((_next, req) =>
    switch (Request.bodyJSON(req)) {
    | Some(body) =>
      switch (Slack.Action.decodeActionType(body)) {
      | InteractiveMessage =>
        body |> Slack.Action.decodeActionBody |> Action.handleAction;
        Response.sendRawStatus(200);
      | UnknownAction => Response.sendStatus(BadRequest)
      }
    | None => Response.sendStatus(BadRequest)
    }
  );

let mayday =
  Middleware.from((_next, req) =>
    switch (Request.bodyJSON(req)) {
    | Some(body) =>
      body |> Slack.Mayday.mapToEvent |> Event.handleEvent;
      Response.sendRawStatus(200);
    | None => Response.sendStatus(BadRequest)
    }
  );

let getToken = req => {
  let headers =
    Belt.Option.getWithDefault(
      Js.Dict.get(Request.asJsonObject(req), "headers"),
      Js.Json.null,
    );

  Json.Decode.(headers |> field("authorization", string));
};

let api =
  PromiseMiddleware.from((_next, req, res) =>
    Js.Promise.(
      switch (Request.bodyJSON(req)) {
      | Some(body) =>
        let token = req |> getToken;

        Slack.IO.authTest(ServerHelpers.getBearerToken(token))
        |> then_(response =>
             body
             |> Slack.Api.mapToEvent
             |> Event.handleApiEvent(~user=response##user)
             |> then_(data => {
                  let json = Js.Dict.empty();
                  Js.Dict.set(json, "data", data);
                  Response.sendJson(Js.Json.object_(json), res) |> resolve;
                })
           );
      | None => Js.Promise.resolve(Response.sendStatus(BadRequest, res))
      }
    )
  );
