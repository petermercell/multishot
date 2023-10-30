# menu GRTools
import nuke

MAX_INPUTS = 16

default_left = '[value parent.nukeShot]'

def switchOPInputChanged():
    n = nuke.thisNode()
    k = nuke.thisKnob()
    
    
    if k.name() == 'inputChange':
        if n.inputs()-1 > 1:
            for i in range(2,MAX_INPUTS):
                count = i-1
                if n.input(count) == None:
                    n.knob('leftvalue{}'.format(i)).setVisible(False)
                    n.knob('op{}'.format(i)).setVisible(False)
                    n.knob('rightvalue{}'.format(i)).setVisible(False)
                    n.knob('rightvalue{}'.format(i)).setValue('')

                else:
                    n.knob('leftvalue{}'.format(i)).setVisible(True)
                    n.knob('op{}'.format(i)).setVisible(True)
                    n.knob('rightvalue{}'.format(i)).setVisible(True)
 
                
                
def switchOPOnUsercreate():
    n = nuke.thisNode()
    for i in range(2, MAX_INPUTS):
        n.knob('leftvalue{}'.format(i)).setValue(default_left)
        n.knob('leftvalue{}'.format(i)).setFlag(nuke.DISABLED)
        n.knob('op{}'.format(i)).setValue('IN')       
        
def switchOPOncreate():
    if nuke.GUI:
        n = nuke.thisNode()
        for i in range(2,MAX_INPUTS):
            if n.input(i-1) is not None:
                n.knob('leftvalue{}'.format(i)).setVisible(True)
                n.knob('op{}'.format(i)).setVisible(True)
                n.knob('rightvalue{}'.format(i)).setVisible(True)
            if n.knob('leftvalue{}'.format(i)).toScript() == default_left:
                n.knob('leftvalue{}'.format(i)).setFlag(nuke.DISABLED)
        
        

nuke.addKnobChanged(switchOPInputChanged, nodeClass='switchOp')
nuke.addOnUserCreate(switchOPOnUsercreate, nodeClass='switchOp')
nuke.addOnCreate(switchOPOncreate, nodeClass='switchOp')