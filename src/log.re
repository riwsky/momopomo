[%bs.raw {|require('./semantic/semantic.css')|}];

type action =
  | SelectEntry(int)
  | EditEntry(int)
  | Up
  | Down;

type entry = {
  estimated: int,
  soFar: int,
  completed: bool
};

module StringMap =
  Map.Make(
    {
      type t = string;
      let compare = (x1, x2) => String.compare(x1, x2);
    }
  );

type state = {
  entries: StringMap.t(entry),
  selected: int,
  editing: bool
};

let component = ReasonReact.reducerComponent("Log");

let s2e = ReasonReact.stringToElement;

let make = _children => {
  ...component,
  initialState: () => {
    entries:
      StringMap.(
        empty
        |> add(
             "make notifications work",
             {estimated: 2, soFar: 2, completed: true}
           )
        |> add(
             "implement the log",
             {estimated: 2, soFar: 1, completed: false}
           )
        |> add("whistle aimlessly", {estimated: 1, soFar: 0, completed: false})
      ),
    selected: 0,
    editing: false
  },
  subscriptions: self => {
    let upListener = Listeners.forKey("ArrowUp", () => self.send(Up));
    let downListener = Listeners.forKey("ArrowDown", () => self.send(Down));
    [
      Sub(
        () => Listeners.add("keydown", upListener),
        (_) => Listeners.remove("keydown", upListener)
      ),
      Sub(
        () => Listeners.add("keydown", downListener),
        (_) => Listeners.remove("keydown", downListener)
      )
    ];
  },
  reducer: (action, state) => {
    let index =
      switch action {
      | SelectEntry(named) => named
      | Down => min(state.selected + 1, StringMap.cardinal(state.entries) - 1)
      | Up => max(state.selected - 1, 0)
      | EditEntry(index) => index
      };
    ReasonReact.Update({...state, selected: index});
  },
  render: self =>
    <div className="logTable">
      <table className="ui selectable celled table">
        <thead>
          <tr>
            <th> (s2e("Task Name")) </th>
            <th> (s2e("Estimated")) </th>
            <th> (s2e("So Far")) </th>
            <th> (s2e("Done?")) </th>
          </tr>
        </thead>
        {
          let children =
            StringMap.bindings(self.state.entries)
            |> List.stable_sort(((_, e1), (_, e2)) =>
                 compare(e1.completed, e2.completed)
               )
            |> List.mapi((i, (n, e)) => {
                 let selected = i == self.state.selected;
                 <tr
                   className=(selected ? "active " : "")
                   onClick=((_) => self.send(SelectEntry(i)))>
                   <td>
                     <div className="ui ribbon label">
                       <i className="edit icon" />
                     </div>
                     (s2e(n))
                   </td>
                   <td> (s2e(string_of_int(e.estimated))) </td>
                   <td> (s2e(string_of_int(e.soFar))) </td>
                   <td> (s2e(e.completed ? "Yep" : "Not Yet!")) </td>
                 </tr>;
               })
            |> Array.of_list;
          ReasonReact.createDomElement(
            "tbody",
            ~props=Js.Obj.empty(),
            children
          );
        }
      </table>
    </div>
};
