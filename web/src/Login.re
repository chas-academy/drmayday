[@react.component]
let make = () => {
  <a
    href="https://slack.com/oauth/authorize?scope=identity.basic,identity.email,identity.team,identity.avatar&client_id=166988133201.472402506917">
    <img
      alt="Sign in with Slack"
      style={ReactDOMRe.Style.make(~height="40", ~width="172", ())}
      src="https://platform.slack-edge.com/img/sign_in_with_slack.png"
    />
  </a>;
};
