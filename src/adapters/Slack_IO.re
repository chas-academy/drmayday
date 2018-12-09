let sendPayload = payload => {
  let request =
    Axios.makeConfigWithUrl(
      ~url="https://slack.com/api/chat.postMessage",
      ~_method="POST",
      ~data=payload,
      ~headers={"Authorization": "Bearer " ++ Config.slackToken},
      (),
    );

  Js.Promise.(Axios.request(request) |> then_(posted => posted |> resolve));
};