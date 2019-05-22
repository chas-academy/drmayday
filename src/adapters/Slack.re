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
    | Keep
    | Remove
    | SelectRoom
    | UnknownAction;

  let decodeAction = action =>
    switch (action) {
    | "keep" => Keep
    | "remove" => Remove
    | "room_list" => SelectRoom
    | _ => UnknownAction
    };

  type actionItem = {
    action: actions,
    value: option(string),
    selectedOption: option(list(string)),
  };

  let decodeSelected = json => Json.Decode.(json |> field("value", string));

  type t = {
    actions: array(actionItem),
    callbackId: string,
    channel: string,
  };

  type actionType =
    | InteractiveMessage
    | UnknownAction;

  let decodeType = actionString =>
    switch (actionString) {
    | "interactive_message" => InteractiveMessage
    | _ => UnknownAction
    };

  let actionItemToRecord = json => {
    Json.Decode.{
      action: json |> field("name", string) |> decodeAction,
      value: json |> optional(field("value", string)),
      selectedOption:
        json |> optional(field("selected_options", list(decodeSelected))),
    };
  };

  let actionToRecord = json =>
    Json.Decode.{
      actions: json |> field("actions", array(actionItemToRecord)),
      callbackId: json |> field("callback_id", string),
      channel: json |> at(["channel", "id"], string),
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
        command: Commands.Mayday,
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
  let send = (channel: string, message: string) =>
    {"channel": channel, "text": message, "mrkdwn": true} |> IO.sendPayload;

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
      "fallback": "Seems like your interface does not support attachments.",
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

  let specifyRoom = itemId => {j|[
  {
    "fallback": "Seems like your interface does not support attachments.",
    "callback_id": "specify_room",
    "color": "#3AA3E3",
    "attachment_type": "default",
    "callback_id": "$itemId",
    "actions": [
      {
        "name": "room_list",
        "text": "Select room...",
        "type": "select",
        "options": [
          {"text": "Föreläsningssalen", "value": "lounge"},
          {"text": "Första grupprummet", "value": "group_room_1"},
          {"text": "Andra grupprummet", "value": "group_room_2"},
          {"text": "Tredje grupprummet", "value": "group_room_3"},
          {"text": "Fjärde grupprummet", "value": "group_room_4"},
          {"text": "Manowar-rummet", "value": "manowar_room"},
          {"text": "Köket", "value": "kitchen"}
        ]
      }
    ]
  }
  ]
  |j};

  let parseRoom = room =>
    switch (room) {
    | "lounge" => {js|"Föreläsningssalen"|js}
    | "group_room_1" => {js|"Första grupprummet"|js}
    | "group_room_2" => {js|"Andra grupprummet"|js}
    | "group_room_3" => {js|"Tredje grupprummet"|js}
    | "group_room_4" => {js|"Fjärde grupprummet"|js}
    | "manowar_room" => {js|"Manowar-rummet"|js}
    | "kitchen" => {js|"Köket"|js}
    | _ => {js|"Något utav alla rum..."|js}
    };

  let helpMessage =
    [
      "*Sure, here's the list of the all the current commands! :tada:*\n",
      "\n*Student:*\n",
      "```",
      "mayday|m <Description> ________ Call for a teacher.",
      "queue|q _______________________ Show the list of patients",
      "remove ________________________ Remove yourself from the queue",
      "room __________________________ Change your current room",
      "```",
      "\n*Teacher:*\n",
      "```",
      "clear _________________________ Clears the queue of all unfinished items",
      "next __________________________ Get the next in line",
      "queue|q _______________________ Show the list of patients",
      "```",
    ]
    |> String.concat("\n");
};

module Utils = {
  let encodeUserId = id => "<@" ++ id ++ ">";
};