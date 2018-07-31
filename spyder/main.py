import queue
from lib import imaScript

htmlStart="http://www.gamersky.com/"

script = imaScript.imaScript(htmlStart,'image\\',imaMaxCount=10050,htmlMaxCount=1000)
script.run(66)