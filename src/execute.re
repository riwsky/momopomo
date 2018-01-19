/* Timer component and associated state */

[%bs.raw {|require('./semantic/semantic.css')|}];

/* The states the timer component can be in. States with an active countdown
(when playing or on break) carry a Date instead of a Duration because we aren't
guaranteed that our timers will fire exactly once every second.

See render, below.
*/
type status =
  | Ready
  | PlayingSince(Luxon.DateTime.t)
  | PausedAfter(Luxon.Duration.t)
  | BreakingSince(Luxon.DateTime.t);

type state = {
  now: Luxon.DateTime.t,
  status
};

/* The type of events this component has to deal with (see reduce, below) */
type action =
  | Play
  | Pause
  | Stop
  | Toggle
  | Tick(Luxon.DateTime.t);

/* The info we need to make a button module */
module type ButtonInfo = {
  let getAction: action;
  let getColor: string;
  let getIconName: string;
  let getName: string;
};

/* cut down on some boilerplate */
let s2e = ReasonReact.stringToElement;

/* Create a semantic ui button with an icon */
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

let breakDuration: Luxon.Duration.t = Luxon.Duration.fromMillis(1000 * 60 * 5);

/* these will just silently fail otherwise */
Notifications.requestPermission((_) => ());

let component = ReasonReact.reducerComponent("Execute");

/* some formatting helper functions */
let timeLeft: state => Luxon.Duration.t =
  ({now, status}) => {
    let leftWith = (duration, someTime) =>
      Luxon.Duration.minus(duration, Luxon.DateTime.diff(now, someTime));
    switch status {
    | Ready => pomoDuration
    | PlayingSince(someTime) => leftWith(pomoDuration, someTime)
    | BreakingSince(someTime) => leftWith(breakDuration, someTime)
    | PausedAfter(someDur) => Luxon.Duration.minus(pomoDuration, someDur)
    };
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
        | BreakingSince(_) => Stop
        | _ => Play
        };
      ReasonReact.SideEffects((self => self.send(action)));
    | Tick(newNow) =>
      let past = (sinceTime, duration) =>
        Luxon.DateTime.(
          asEpochMillis(sinceTime) <= asEpochMillis(minus(newNow, duration))
        );
      let timeString = mmSs(timeLeft({now:newNow, status:state.status}));
      switch state.status {
      | PlayingSince(someTime) when past(someTime, pomoDuration) =>
        ReasonReact.UpdateWithSideEffects(
          {now: newNow, status: BreakingSince(newNow)},
          (
            (_) => {
              Notifications.create_notification("Timer finished!");
              Audio.go();
              Js.log("Timer finished!");
            }
          )
        )
      | BreakingSince(someTime) when past(someTime, breakDuration) =>
        ReasonReact.UpdateWithSideEffects(
          {now: newNow, status: Ready},
          ((_) => Notifications.create_notification("Break finished!"))
        )
      | _ =>
        ReasonReact.UpdateWithSideEffects(
          {now: newNow, status: state.status},
          ((_) => Notifications.set_title(timeString))
        )
      };
    | Play =>
      switch state.status {
      | Ready =>
        ReasonReact.Update({...state, status: PlayingSince(state.now)})
      | PlayingSince(_)
      | BreakingSince(_) => ReasonReact.NoUpdate
      | PausedAfter(dur) =>
        ReasonReact.Update({
          ...state,
          status: PlayingSince(Luxon.DateTime.minus(state.now, dur))
        })
      }
    | Pause =>
      switch state.status {
      | Ready
      | PausedAfter(_)
      | BreakingSince(_) => ReasonReact.NoUpdate
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
        | BreakingSince(_) => <StopButton sender=self.send />
        | PausedAfter(_) =>
          <div>
            <PlayButton sender=self.send />
            <StopButton sender=self.send />
          </div>
        }
      )
    </div>
};
