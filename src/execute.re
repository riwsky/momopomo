[%bs.raw {|require('./semantic/semantic.css')|}];

let s2e = ReasonReact.stringToElement;

type action =
  | Play
  | Pause
  | Stop
  | Toggle
  | Tick(Luxon.DateTime.t);

module type ButtonInfo = {
  let getAction: action;
  let getColor: string;
  let getIconName: string;
  let getName: string;
};

module Button = (Info: ButtonInfo) => {
  let component = ReasonReact.statelessComponent(Info.getName ++ "Button");
  let make = (~sender, _children) => {
    ...component,
    render: _self =>
      <button
        className=("ui labeled icon button " ++ Info.getColor)
        onClick=(_event => sender(Info.getAction))>
        <i className=(Info.getIconName ++ " icon") />
        (s2e(Info.getName))
      </button>
  };
};

module PlayButton =
  Button(
    {
      let getAction = Play;
      let getColor = "blue";
      let getIconName = "play";
      let getName = "Play";
    }
  );

module StopButton =
  Button(
    {
      let getAction = Stop;
      let getColor = "red";
      let getIconName = "stop";
      let getName = "Stop";
    }
  );

module PauseButton =
  Button(
    {
      let getAction = Pause;
      let getColor = "gray";
      let getIconName = "pause";
      let getName = "Pause";
    }
  );

let pomoDuration: Luxon.Duration.t = Luxon.Duration.fromMillis(1000 * 60 * 25);

Notifications.requestPermission((_) => ());

type status =
  | Ready
  | PlayingSince(Luxon.DateTime.t)
  | PausedAfter(Luxon.Duration.t);

type state = {
  now: Luxon.DateTime.t,
  status
};

let component = ReasonReact.reducerComponent("Execute");

let timeLeft: state => Luxon.Duration.t =
  ({now, status}) =>
    switch status {
    | Ready => pomoDuration
    | PlayingSince(someTime) =>
      Luxon.Duration.minus(pomoDuration, Luxon.DateTime.diff(now, someTime))
    | PausedAfter(someDur) => Luxon.Duration.minus(pomoDuration, someDur)
    };

let mmSs: Luxon.Duration.t => string =
  dur => Luxon.Duration.toFormat(dur, "mm:ss");

let make = _children => {
  ...component,
  initialState: () => {now: Luxon.DateTime.localNow(), status: Ready},
  subscriptions: self => {
    let listener = event =>
      if (event##key == " ") {
        event##preventDefault();
        self.send(Toggle);
      };
    [
      Sub(
        () =>
          Js.Global.setInterval(
            () => self.send(Tick(Luxon.DateTime.localNow())),
            1000
          ),
        Js.Global.clearInterval
      ),
      Sub(
        () => Listeners.add("keydown", listener),
        (_) => Listeners.remove("keydown", listener)
      )
    ];
  },
  reducer: (action, state) =>
    switch action {
    | Toggle =>
      let action =
        switch state.status {
        | PlayingSince(_) => Pause
        | _ => Play
        };
      ReasonReact.SideEffects((self => self.send(action)));
    | Tick(newNow) =>
      switch state.status {
      | PlayingSince(someTime)
          when
            Luxon.DateTime.(
              asEpochMillis(someTime)
              <= asEpochMillis(minus(newNow, pomoDuration))
            ) =>
        ReasonReact.UpdateWithSideEffects(
          {now: newNow, status: Ready},
          (
            (_) => {
              Notifications.create_notification("Timer finished!");
              Audio.go();
            }
          )
        )
      | _ => ReasonReact.Update({now: newNow, status: state.status})
      }
    | Play =>
      switch state.status {
      | Ready =>
        ReasonReact.Update({...state, status: PlayingSince(state.now)})
      | PlayingSince(_) => ReasonReact.NoUpdate
      | PausedAfter(dur) =>
        ReasonReact.Update({
          ...state,
          status: PlayingSince(Luxon.DateTime.minus(state.now, dur))
        })
      }
    | Pause =>
      switch state.status {
      | Ready
      | PausedAfter(_) => ReasonReact.NoUpdate
      | PlayingSince(someTime) =>
        ReasonReact.Update({
          ...state,
          status: PausedAfter(Luxon.DateTime.diff(state.now, someTime))
        })
      }
    | Stop => ReasonReact.Update({...state, status: Ready})
    },
  render: self =>
    <div>
      <h1> (s2e(mmSs(timeLeft(self.state)))) </h1>
      (
        switch self.state.status {
        | Ready => <PlayButton sender=self.send />
        | PlayingSince(_) =>
          <div>
            <PauseButton sender=self.send />
            <StopButton sender=self.send />
          </div>
        | PausedAfter(_) =>
          <div>
            <PlayButton sender=self.send />
            <StopButton sender=self.send />
          </div>
        }
      )
      Audio.tag
    </div>
};
