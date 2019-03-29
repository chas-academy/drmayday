let component = ReasonReact.statelessComponent(__MODULE__);

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

  let time_ago = [%css
    [
      alignItems(`center),
      backgroundColor(`hex(Typography.warning_light)),
      borderRadius(`px(5)),
      color(`hex(Typography.warning_dark)),
      display(`flex),
      height(`px(22)),
      marginLeft(`auto),
      padding2(`px(4), `px(16)),
    ]
  ];
};
let get_color = status => {
  let (background_color, color) =
    switch (status) {
    | "warning" => (Typography.warning_dark, Typography.warning_light)
    };
  ();
};
type t = {
  name: string,
  room: string,
  description: string,
  time_ago: string,
};

let make = (~item, _children) => {
  ...component,

  render: _self => {
    <li className=Styles.wrap>
      <div className=Styles.inner_wrap>
        <span className=Styles.avatar> {ReasonReact.string("TE")} </span>
        <div className=Styles.name_room>
          <p className=Typography.p_lead> {ReasonReact.string(item.name)} </p>
          <p className=Typography.p_quiet>
            {item.room |> ReasonReact.string}
          </p>
        </div>
        <span className=Styles.time_ago>
          <span className=Typography.small_bold>
            {ReasonReact.string(item.time_ago)}
          </span>
        </span>
      </div>
      <div className=Styles.description_wrap>
        <p className=Typography.p_quiet>
          {ReasonReact.string(item.description)}
        </p>
      </div>
    </li>;
  },
};
