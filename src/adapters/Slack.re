module IO = {
  let sendPayload = payload => {
    let request =
      Axios.makeConfigWithUrl(
        ~url="https://slack.com/api/chat.postMessage",
        ~_method="POST",
        ~data=payload,
        ~headers={"Authorization": "Bearer " ++ Config.slackToken},
        (),
      );

    Js.Promise.(Axios.request(request) |> then_(posted => posted |> resolve));
  };

  let isAdmin = userId => {
    let request =
      Axios.makeConfigWithUrl(
        ~url="https://slack.com/api/users.info?user=" ++ userId,
        ~_method="GET",
        ~headers={"Authorization": "Bearer " ++ Config.slackToken},
        (),
      );

    Js.Promise.(
      Axios.request(request)
      |> then_(user => user |> resolve)
      |> then_(result => result##data##user##is_admin |> resolve)
    );
  };
};

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
    Json.Decode.(
      switch (json |> optional(field("subtype", string))) {
      | Some(subtype) => subtype |> decodeType
      | None => Human
      }
    );
};

module Event = {
  let parseQuery = text =>
    text
    |> Js.String.split(" ")
    |> Js.Array.sliceFrom(1)
    |> Js.Array.joinWith(" ");

  type event = {
    channel: string,
    command: Commands.commands,
    subtype: SubType.subType,
    text: string,
    user: string,
  };

  type eventType =
    | EventCallback
    | UrlVerification
    | UnknownEvent;

  type eventPayload = {
    event: option(event),
    eventType,
  };

  let decodeEvent = json =>
    Json.Decode.{
      channel: json |> field("channel", string),
      command:
        Belt.Option.getWithDefault(
          json |> optional(field("text", string)),
          "",
        )
        |> Commands.decodeCommand,
      text:
        switch (json |> optional(field("text", string))) {
        | Some(text) => text |> parseQuery
        | None => ""
        },
      subtype: json |> SubType.decode,
      user: json |> field("user", string),
    };

  let decodeType = eventString =>
    switch (eventString) {
    | "event_callback" => EventCallback
    | "url_verification" => UrlVerification
    | _ => UnknownEvent
    };

  let decodeEventPayload = json =>
    Json.Decode.{
      eventType: json |> field("type", string) |> decodeType,
      event: json |> optional(field("event", decodeEvent)),
    };

  let decodeEventType = json =>
    json |> Json.Decode.(field("type", string)) |> decodeType;
};

module Action = {
  type actions =
    | Remove
    | UnknownAction;

  let decodeAction = action =>
    switch (action) {
    | "remove" => Remove
    | _ => UnknownAction
    };

  type actionItem = {
    action: actions,
    itemId: option(string),
  };

  type t = {
    actions: array(actionItem),
    callbackId: string,
  };

  type actionType =
    | InteractiveMessage
    | UnknownAction;

  let decodeType = actionString =>
    switch (actionString) {
    | "interactive_message" => InteractiveMessage
    | _ => UnknownAction
    };

  let actionItemToRecord = json =>
    Json.Decode.{
      action: json |> field("name", string) |> decodeAction,
      itemId: json |> optional(field("value", string)),
    };

  let actionToRecord = json =>
    Json.Decode.{
      actions: json |> field("actions", array(actionItemToRecord)),
      callbackId: json |> field("callback_id", string),
    };

  let decodeActionBody = json =>
    Json.Decode.(
      json |> field("payload", string) |> Json.parseOrRaise |> actionToRecord
    );

  let decodeActionType = json =>
    Json.Decode.(
      json
      |> field("payload", string)
      |> Json.parseOrRaise
      |> field("type", string)
      |> decodeType
    );
};

module Mayday = {
  let mapToEvent = json => {
    let event: Event.event =
      Json.Decode.{
        channel: json |> field("channel_id", string),
        command: Commands.Help,
        subtype: json |> SubType.decode,
        text: json |> field("text", string),
        user: json |> field("user_id", string),
      };
    event;
  };
};

module Verification = {
  type t = {
    challenge: string,
    token: string,
  };

  let decode = json =>
    Json.Decode.{
      challenge: json |> field("challenge", string),
      token: json |> field("token", string),
    };

  let challengeToken = body => {
    let {challenge} = body |> decode;
    challenge;
  };
};

module Message = {
  let send = (_channel: string, message: string) =>
    {"channel": Config.slackBotId, "text": message, "mrkdwn": true}
    |> IO.sendPayload;

  let sendWithAttachments = (channel: string, message: string, attachments) =>
    {
      "channel": channel,
      "text": message,
      "attachments": attachments,
      "mrkdwn": true,
    }
    |> IO.sendPayload;

  let confirmQueueRemoval = itemId => {j|[
    {
      "fallback": "Seems like your interface does not suppor attachments.",
      "callback_id": "confirm_finished",
      "color": "#3AA3E3",
      "attachment_type": "default",
      "actions": [
        {
          "name": "remove",
          "text": "Yes",
          "type": "button",
          "value": $itemId,
        },
        {
          "name": "keep",
          "text": "No",
          "type": "button",
          "value": "no",
        },
      ],
    },
  ]
  |j};

  let helpMessage =
    [
      "*Sure, here's the list of the all the current commands! :tada:*\n",
      "\n*Student:*\n",
      "```",
      "mayday <Description> ___________________ Call for a teacher.",
      "queue __________________________________ Show the list of patients",
      "```",
      "\n*Teacher:*\n",
      "```",
      "next ___________________________________ Get the next in line",
      "queue __________________________________ Show the list of patients",
      "```",
    ]
    |> String.concat("\n");
};

module Utils = {
  let encodeUserId = id => "<@" ++ id ++ ">";
};