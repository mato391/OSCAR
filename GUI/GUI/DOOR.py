from Element import Element

class DOOR(Element):

    def __init__(self, name, label, state):
        Element.name = name
        self.label = label
        self.state = state