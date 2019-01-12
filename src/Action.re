let handleAction = actionObj => {
  let {actions, callbackId, channel}: Slack.Action.t = actionObj;

  let sendMessage = Slack.Message.send(channel);

  actions->Belt.Array.map(({action, value, selectedOption}) =>
    switch (action) {
    | Remove => Queue.markAsFinished(value, sendMessage)
    | SelectRoom => Help.selectRoom(selectedOption, callbackId, sendMessage)
    | _ => ()
    }
  )
  |> ignore;

  ();
};