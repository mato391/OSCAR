from DOOR import DOOR

class SiteConfCreator:

    doors = []

    def __init__(self, siteconf_raw):
        self.siteconf_raw = siteconf_raw
        self.getDoors()
        print str(len(self.doors)) + " has been created"

    def getDoors(self):
        for element in self.siteconf_raw:
            if element.find("DOOR") != -1:
                tmp = element.split(":")
                name = tmp[0]
                label = tmp[1]
                state = tmp[2]
                print "Creating " + name + " with label " + label + " with state " + state
                self.doors.append(DOOR(name, label, state))
