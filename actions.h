#ifndef actions_h
#define actions_h

enum Action {
  Stop=0,
  Reset=1,
  PrimeMeat=2,
  DropMeat=3
};

enum ActionEvent {
  Stopped=0,
  ResetStarted=1,
  ResetFinished=2,
  PrimeMeatStarted=3,
  PrimeMeatFinished=4,
  DropMeatStarted=5,
  DropMeatFinished=6,
  MeatEmpty=7
};
//should be a get state

enum PerchEvent {
  PerchVacant=0,
  PerchOccupied=1
};

enum HeartBeat {
  Init=0,
  Pulse=1
};


#endif
