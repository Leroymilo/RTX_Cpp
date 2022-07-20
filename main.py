import wx
from video_builder import Window

import imageio
from PIL import Image
import cv2
from os import listdir

class VideoBuilder(Window) :
    def __init__(self) :
        super().__init__(None)
    
    def build(self, event) :
        self.button.Disable()
        if self.textCtrl_name.GetValue().strip() == '' :
            name = "result"
        else :
            name = self.textCtrl_name.GetLabel()
        for pos_ext in self.choice_ext.GetStrings() :
            name.removesuffix(pos_ext)
        ext = self.choice_ext.GetStringSelection()
        fps = self.spinCtrl_fps.GetValue()

        frames = ["renders\\"+f for f in listdir("renders")]

        if ext == ".gif" :
            writer = imageio.get_writer(
                name + ext,
                duration = 1/fps    #Duration of a frame in seconds
                )
            
            for frame in frames :
                writer.append_data(imageio.imread(frame))
            writer.close()
        
        else :
            size = Image.open(frames[0]).size
            writer = cv2.VideoWriter(name + ext, 0, fps, size)

            for frame in frames :
                writer.write(cv2.imread(frame))
            cv2.destroyAllWindows()
            writer.release()
        self.button.Enable()

app = wx.App(False)
frame = VideoBuilder()
frame.Show()
app.MainLoop()