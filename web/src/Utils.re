open Dom.Storage;

let has_token = () =>
  switch (localStorage |> getItem("slackToken")) {
  | Some(t) => true
  | None => false
  };
