module Token = {
  open Dom.Storage;

  let get_token = () => {
    switch (localStorage |> getItem("slackToken")) {
    | Some(token) => token
    | None => ""
    };
  };

  let has_token = () =>
    switch (localStorage |> getItem("slackToken")) {
    | Some(_t) => true
    | None => false
    };

  let set_token = token =>
    localStorage |> setItem("slackToken", token) |> ignore;
};

module Api = {
  open Js.Promise;

  let headers =
    Axios.Headers.fromObj({"Authorization": "Bearer " ++ Token.get_token()});

  let me = () => {
    Axios.getc("/api/me", Axios.makeConfig(~headers, ()))
    |> then_(response => resolve(Js.log(response##data)))
    |> catch(error => resolve(Js.log(error)));
  };

  let queue = callback => {
    Js.Promise.(
      Axios.postDatac(
        "/api/command",
        {
          {
            "channel_id": "",
            "command": "queue",
            "subtype": "human",
            "args": "",
          };
        },
        Axios.makeConfig(~headers, ()),
      )
      |> then_(response => resolve(callback(response##data##data)))
      |> catch(error => resolve(Js.log(error)))
    );
  };
};
