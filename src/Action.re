let handleAction = actionObj => {
  let {actions, callbackId}: Slack.Action.t = actionObj;

  let sendMessage = Slack.Message.send("");

  actions->Belt.Array.map(({action, value, selectedOption}) =>
    switch (action) {
    | Remove => Queue.markAsFinished(value)
    | SelectRoom => Help.selectRoom(selectedOption, callbackId, sendMessage)
    | _ => ()
    }
  )
  |> ignore;

  ();
};