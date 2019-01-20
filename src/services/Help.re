let mayday = (text, user, isAdmin, sendMessage, sendMessageWithAttachments) => {
  Js.Promise.(
    if (isAdmin) {
      sendMessage("Hey, this is restricted to students!") |> ignore
    } else {
      Database.hasUnfinishedHelpItem(user)
      |> then_(((hasUnfinished, _itemId)) => {

            if (hasUnfinished) {
              sendMessage("You're already on the help list!") |> ignore
            } else {

              Database.addHelpItem(user, text) 
              |> then_(itemId => 
                sendMessageWithAttachments(
                  "OK, I've added you to the queue, but please tell me which room you're in: "
                  ++ Slack.Utils.encodeUserId(user), 
                  Slack.Message.specifyRoom(itemId)
                )
              ) |> ignore

            } 

            resolve();
        }) |> ignore
    }
  );
};

let selectRoom = (selectOption, itemId, sendMessage) => {
  switch (selectOption) {
  | Some(room) =>
    Database.addRoom(int_of_string(itemId), List.hd(room), sendMessage)
  | None => ()
  };
};

let updateRoom = (user, isAdmin, sendMessage, sendMessageWithAttachments) => {
  Js.Promise.(
    isAdmin ? (
      sendMessage("Hey, this is restricted to students!") |> ignore
    ) : (
      Database.hasUnfinishedHelpItem(user)
      |> then_(((hasUnfinished, itemId)) => {
        
            if (hasUnfinished) {
              sendMessageWithAttachments(
                "OK, please tell me which room you're in: ",
                Slack.Message.specifyRoom(itemId)
              )
            } else {
              sendMessage("Hmm, you don't seem to be in the queue. Use `mayday` to add yourself.")
            } 

            resolve();
        }) |> ignore
    )
  )
}