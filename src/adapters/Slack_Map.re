let challenge = json =>
  Json.Decode.{
    challenge: json |> field("challenge", string),
    token: json |> field("token", string),
  };

let eventType = json =>
  json |> Json.Decode.(field("type", string)) |> Decode.Event.decodeType;

let actionType = json =>
  Json.Decode.(
    json
    |> field("payload", string)
    |> Utils.parseToJson
    |> field("type", string)
    |> Decode.Action.decode
  );

let subType = json =>
  json |> Json.Decode.(field("subtype", string)) |> Decode.SubType.decodeType;