let getDictString = (dict, key) =>
  switch (Js.Dict.get(dict, key)) {
  | Some(json) => Js.Json.decodeString(json)
  | _ => None
  };

let setProperty = (req, property, value) => {
  let reqData = Express.Request.asJsonObject(req);
  Js.Dict.set(reqData, property, value);
};

let getBearerToken = token =>
  token |> Js.String.(slice(~from=7, ~to_=length(token)));

let makeUserObject = userId => {
  let dict = Js.Dict.empty();
  Js.Dict.set(dict, "user", Js.Json.string(userId));

  dict |> Js.Json.object_;
};