module Styles = {
  open Emotion;

  let wrap = [%css
    [
      alignItems(`flexStart),
      display(`flex),
      flexDirection(`column),
      marginTop(`px(192)),
    ]
  ];

  let item_list = [%css
    [margin(`px(0)), padding(`px(0)), width(`pct(100.0))]
  ];

  let opacity_list = [%css [opacity(0.6)]];
  let item_list_upcoming = Cx.merge([|item_list, opacity_list|]);
};

let tom: HelpItem.t = {
  name: "John Doe",
  room: "Manowar-rummet",
  description: {j|Förstår inte vad som händer.. Försöker köra vagrant up, men får bara massa felmeddelanden.|j},
  time_ago: "2 hours",
};

let help_items = [tom, tom, tom];

[@react.component]
let make = () => {
  let url = ReasonReactRouter.useUrl();

  let showDashboard = () => <Dashboard />;
  let showLogin = () => <Login />;

  switch (url.path, Utils.Token.has_token()) {
  | (["login", token], false) =>
    Utils.Token.set_token(token);
    ReasonReactRouter.push("/");
    showDashboard();
  | (_, false) => showLogin()
  | (_, true) => showDashboard()
  };
};
