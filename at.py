class node:
    def __init__(self, text):
        self.text = text
        self.children = None

object = node("asdf")
object.children = (5,1,2)
object.parent = 5

print object.parent
