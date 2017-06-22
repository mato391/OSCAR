import wx
from SiteConfCreator import SiteConfCreator
from DOORInformator import DOORInformator
class MainGUI(wx.Frame):

    def __init__(self, parent, title):
        super(MainGUI, self).__init__(parent, title=title,
                                      size=(400, 400))
        self.getSiteConf()
        self.siteConfCreator = SiteConfCreator(self.siteconfRaw)
        self.doors = self.siteConfCreator.doors
        print "Got doors: ", self.doors
        self.InitUI()

    def OnNext(self, e):
        self.newInfo = wx.StaticText(self.pnl, label="NEW WIDGET")
        self.hbox = wx.BoxSizer(wx.HORIZONTAL)
        self.fgs = wx.FlexGridSizer(3, 3, 50, 50)
        self.fgs.AddMany([(self.clock), (self.wrap), (self.temp), (self.leftCentrWrap), (self.newInfo, 1, wx.EXPAND),
                          (self.rightCentrWrap), (self.prevButton), (self.bottomWrap, 1, wx.EXPAND), (self.nextButton)])
        self.hbox.Add(self.fgs)
        self.pnl.SetSizer(self.hbox)
        self.pnl.Refresh()

    def InitUI(self):
        '''self.getAppConf()
        #self.prepareCreateListOfWidgets()
        pnl = wx.Panel(self)
        #vbox = wx.BoxSizer(wx.VERTICAL)
        #self.infoMsgPanel = wx.StaticText(pnl, style=wx.ALIGN_CENTRE)
        gs = wx.GridSizer()
        self.getDoorsSummaryInfo()
        #vbox.Add(self.infoMsgPanel, flag=wx.ALL, border=5)
        pnl.SetSizer(vbox)
        #self.Bind(wx.EVT_TOOL, self.OnQuit, qtool)'''
        self.pnl = wx.Panel(self)
        self.hbox = wx.BoxSizer(wx.HORIZONTAL)
        self.fgs = wx.FlexGridSizer(3, 3, 50, 50)
        self.clock = wx.StaticText(self.pnl, label="5:03")
        self.wrap = wx.StaticText(self.pnl, label="")
        self.temp = wx.StaticText(self.pnl, label="9st")

        self.leftCentrWrap = wx.StaticText(self.pnl, label="")
        self.doorsInfo = wx.StaticBitmap(self.pnl, bitmap=wx.EmptyBitmap(100, 100))
        self.rightCentrWrap = wx.StaticText(self.pnl, label="")

        self.nextButton = wx.Button(self.pnl, -1, "NEXT")
        self.nextButton.Bind(wx.EVT_BUTTON, self.OnNext)
        self.bottomWrap = wx.StaticText(self.pnl, label="")
        self.prevButton = wx.Button(self.pnl, -1, "PREV")

        self.fgs.AddMany([(self.clock), (self.wrap), (self.temp), (self.leftCentrWrap), (self.doorsInfo, 1, wx.EXPAND), (self.rightCentrWrap), (self.prevButton), (self.bottomWrap, 1, wx.EXPAND), (self.nextButton)])

        #fgs.AddGrowableRow(2, 1)
        #fgs.AddGrowableCol(1, 1)

        self.hbox.Add(self.fgs, proportion=1, flag=wx.ALL|wx.EXPAND, border=15)
        self.pnl.SetSizer(self.hbox)
        self.Centre()
        self.Show(True)

    def getAppConf(self):
        f = open("D:\\private\\OSCAR\\New_Architecture_OSCAR\\OSCAR\\config\\gui_conf.txt")
        self.gui_conf = f.readlines()
        f.close()

    def prepareCreateListOfWidgets(self):
        for element in self.gui_conf:
            if element.find("DOORStatus"):
                pass


    def getSiteConf(self):
        f = open("D:\\private\\OSCAR\\New_Architecture_OSCAR\\OSCAR\\config\\siteconf.txt")
        self.siteconfRaw = f.readlines()
        f.close()

    def getDoorsSummaryInfo(self):
        self.doorsStatus = DOORInformator.getDoorsSummary(self.doors)
        print "getDoorsSummaryInfo: ", self.doorsStatus
        if self.doorsStatus != False:
            msg = ""
            for door in self.doorsStatus:
                msg += door.label + ", "
            self.infoMsgPanel.SetLabelText("Doors: " + msg + " are opened")


    def OnQuit(self, e):
        self.Close()

def main():

    ex = wx.App()
    MainGUI(None, "MAIN")
    ex.MainLoop()


if __name__ == '__main__':
    main()