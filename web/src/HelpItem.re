module Styles = {
  open Emotion;

  let wrap = [%css
    [
      backgroundColor(`hex(Typography.light_background)),
      boxShadow(~y=px(5), ~blur=px(12), `hsla((198, 100, 15, 0.14))),
      borderRadius(`px(5)),
      listStyleType(`none),
      margin(`px(0)),
      marginBottom(`px(32)),
      padding(`px(0)),
    ]
  ];

  let inner_wrap = [%css
    [
      alignItems(`center),
      backgroundColor(`hex("FFFFFF")),
      borderRadius(`px(5)),
      display(`flex),
      padding4(`px(12), `px(32), `px(12), `px(16)),
    ]
  ];

  let description_wrap = [%css
    [
      borderTop(`px(1), `solid, `hex("EBF7FF")),
      padding2(`px(8), `px(16)),
    ]
  ];

  let name_room = [%css
    [display(`flex), flexDirection(`column), justifyContent(`center)]
  ];

  let avatar = [%css
    [
      borderRadius(`px(96)),
      height(`px(36)),
      width(`px(36)),
      marginRight(`px(12)),
    ]
  ];

  let time_ago = ((bgColor, textColor)) => [%css
    [
      alignItems(`center),
      backgroundColor(`hex(bgColor)),
      borderRadius(`px(5)),
      color(`hex(textColor)),
      display(`flex),
      height(`px(22)),
      marginLeft(`auto),
      padding2(`px(4), `px(16)),
    ]
  ];
};

let get_color =
  fun
  | "warning" => (Typography.warning_light, Typography.warning_dark)
  | "danger" => (Typography.danger_light, Typography.danger_dark);

type t = {
  name: string,
  room: string,
  description: string,
  time_ago: string,
};

module Decode = {
  let helpItem = json =>
    Json.Decode.{
      description: json |> field("description", string),
      name: json |> field("user_id", string),
      room: json |> field("room", string),
      time_ago: json |> field("time_created", int) |> string_of_int,
    };

  let helpItems = items => Json.Decode.(items |> list(helpItem));
};

[@react.component]
let make = (~item) => {
  <li className=Styles.wrap>
    <div className=Styles.inner_wrap>
      <span className=Styles.avatar> {React.string("TE")} </span>
      <div className=Styles.name_room>
        <p className=Typography.p_lead> {React.string(item.name)} </p>
        <p className=Typography.p_quiet> {item.room |> React.string} </p>
      </div>
      <span
        className={Styles.time_ago((
          Typography.warning_light,
          Typography.warning_dark,
        ))}>
        <span className=Typography.small_bold>
          {React.string(item.time_ago)}
        </span>
      </span>
    </div>
    <div className=Styles.description_wrap>
      <p className=Typography.p_quiet> {React.string(item.description)} </p>
    </div>
  </li>;
};
