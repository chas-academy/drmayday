module SubType = {
  type subType =
    | Bot
    | Human;

  let decodeType = subTypeString =>
    switch (subTypeString) {
    | "bot_message" => Bot
    | _ => Human
    };

  let decode = json =>
    json |> Json.Decode.(field("subtype", string)) |> decodeType;
};

module Event = {
  type t = {
    channel: string,
    command: Commands.commands,
    subtype: SubType.subType,
    text: string,
    timestamp: string,
    user: string,
  };

  type eventType =
    | EventCallback
    | UrlVerification
    | UnknownEvent;

  let decodeType = eventString =>
    switch (eventString) {
    | "event_callback" => EventCallback
    | "url_verification" => UrlVerification
    | _ => UnknownEvent
    };

  let eventType = json =>
    json |> Json.Decode.(field("type", string)) |> decodeType;
};

module Action = {
  type actionItem = {
    command: Commands.commands,
    itemId: option(string),
  };

  type t = {
    actions: array(actionItem),
    callbackId: string,
  };

  type actionType =
    | InteractiveMessage
    | UnknownAction;

  let decode = actionString =>
    switch (actionString) {
    | "interactive_message" => InteractiveMessage
    | _ => UnknownAction
    };

  let actionType = json =>
    Json.Decode.(
      json
      |> field("payload", string)
      |> Json.parseOrRaise
      |> field("type", string)
      |> decode
    );
};

module Utils = {
  type verification = {
    challenge: string,
    token: string,
  };

  let decodeVerification = json =>
    Json.Decode.{
      challenge: json |> field("challenge", string),
      token: json |> field("token", string),
    };
};