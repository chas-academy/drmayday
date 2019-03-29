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

let component = ReasonReact.statelessComponent(__MODULE__);

let tom: HelpItem.t = {
  name: "John Doe",
  room: "Manowar-rummet",
  description: {j|Förstår inte vad som händer.. Försöker köra vagrant up, men får bara massa felmeddelanden.|j},
  time_ago: "2 hours",
};

let help_items = [tom, tom, tom];

let make = _children => {
  ...component,
  render: _self => {
    let next = help_items |> List.hd;
    let upcoming = help_items |> List.tl;

    Utils.has_token()
      ? <div className=Styles.wrap>
          <h1 className=Typography.h1> {ReasonReact.string("Next")} </h1>
          <ul className=Styles.item_list> <HelpItem item=next /> </ul>
          <h1 className=Typography.h1> {ReasonReact.string("Upcoming")} </h1>
          <ul className=Styles.item_list_upcoming>
            {ReasonReact.array(
               Array.of_list(
                 upcoming
                 |> List.mapi((i, hi) =>
                      <HelpItem key={string_of_int(i)} item=hi />
                    ),
               ),
             )}
          </ul>
        </div>
      : <a
          href="https://slack.com/oauth/authorize?scope=identity.basic,identity.email,identity.team,identity.avatar&client_id=166988133201.472402506917">
          <img
            alt="Sign in with Slack"
            style={ReactDOMRe.Style.make(~height="40", ~width="172", ())}
            src="https://platform.slack-edge.com/img/sign_in_with_slack.png"
          />
        </a>;
  },
};
