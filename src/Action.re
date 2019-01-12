let handleAction = actionObj => {
  let {actions, callbackId}: Slack.Action.t = actionObj;

  let sendMessage = Slack.Message.send("");

  actions->Belt.Array.map(({action, value, selectedOption}) =>
    switch (action) {
    | Remove =>
      switch (value) {
      | Some(id) => Database.closeHelpItem(int_of_string(id))
      | None => ()
      }
    | SelectRoom =>
      switch (selectedOption) {
      | Some(room) =>
        Database.addRoom(
          int_of_string(callbackId),
          List.hd(room),
          sendMessage,
        )
      | None => ()
      }
    | _ => ()
    }
  )
  |> ignore;

  ();
};