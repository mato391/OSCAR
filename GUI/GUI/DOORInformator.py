
class DOORInformator:

    @staticmethod
    def getDoorsSummary(doors):
        openedDoors = []
        for door in doors:
            print "getDoorsSummary: " + door.label + " is " + door.state
            if door.state.find("OPENED") != -1:
                openedDoors.append(door)
        print "DOORInformator: ", len(openedDoors)
        if len(openedDoors) != 0:
            return openedDoors
        else:
            return False