let handleAction = actionObj => {
  let {actions}: Slack.Action.t = actionObj;

  actions
  ->Belt.Array.map(({action, itemId}) =>
      switch (action) {
      | Remove =>
        switch (itemId) {
        | Some(id) => Database.closeHelpItem(int_of_string(id))
        | None => ()
        }
      | _ => ()
      }
    )
  |> ignore;

  ();
};