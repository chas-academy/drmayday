let mayday = (text, user, isAdmin, sendMessage, sendMessageWithAttachments) => {
  Js.Promise.(
    isAdmin ?
      sendMessage("Hey, this is restricted to students!") |> ignore :
      user
      |> Database.hasUnfinishedHelpItem
      |> then_(hasUnfinished => {
           hasUnfinished ?
             sendMessage("You're already on the help list!") |> ignore :
             Database.addHelpItem(user, text, sendMessageWithAttachments);

           resolve();
         })
      |> ignore
  );
};