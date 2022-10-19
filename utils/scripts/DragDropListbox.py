
import Tkinter as Tkinter

from pygubu import BuilderObject, register_widget

class DragDropListbox(Tkinter.Listbox):
    """ A Tkinter listbox with drag'n'drop reordering of entries. """
    def __init__(self, master, **kw):
        kw['selectmode'] = Tkinter.SINGLE
        Tkinter.Listbox.__init__(self, master, kw)
        self.bind('<Button-1>', self.setCurrent)
        self.bind('<B1-Motion>', self.shiftSelection)
        self.curIndex = None

    def setCurrent(self, event):
        self.curIndex = self.nearest(event.y)

    def shiftSelection(self, event):
        i = self.nearest(event.y)
        if i < self.curIndex:
            x = self.get(i)
            self.delete(i)
            self.insert(i+1, x)
            self.curIndex = i
        elif i > self.curIndex:
            x = self.get(i)
            self.delete(i)
            self.insert(i-1, x)
            self.curIndex = i 

class TKDragDropListbox(BuilderObject):
    class_ = DragDropListbox
    container = False
    OPTIONS_STANDARD = (
        'background', 'borderwidth', 'cursor',
        'disabledforeground', 'exportselection', 'font',
        'foreground',  'highlightbackground', 'highlightcolor',
        'highlightthickness',  'relief',
        'selectbackground', 'selectborderwidth', 'selectforeground',
        'setgrid', 'takefocus',  'xscrollcommand', 'yscrollcommand')
    OPTIONS_SPECIFIC = ('activestyle', 'height', 'listvariable',
                        'selectmode', 'state', 'width')
    properties = OPTIONS_STANDARD + OPTIONS_SPECIFIC
    command_properties = ('xscrollcommand', 'yscrollcommand')

register_widget('tk.DragDropListbox', TKDragDropListbox,
                'DragDropListbox', ('Control & Display', 'tk'))            
            