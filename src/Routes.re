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
            Response.sendStatus(Ok);
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
        Response.sendStatus(Ok);
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
      Response.sendStatus(Ok);
    | None => Response.sendStatus(BadRequest)
    }
  );