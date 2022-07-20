# -*- coding: utf-8 -*- 

###########################################################################
## Python code generated with wxFormBuilder (version Jun 17 2015)
## http://www.wxformbuilder.org/
##
## PLEASE DO "NOT" EDIT THIS FILE!
###########################################################################

import wx
import wx.xrc

###########################################################################
## Class Window
###########################################################################

class Window ( wx.Frame ):
	
	def __init__( self, parent ):
		wx.Frame.__init__ ( self, parent, id = wx.ID_ANY, title = u"Video Builder", pos = wx.DefaultPosition, size = wx.Size( 300,110 ), style = wx.CAPTION|wx.CLOSE_BOX|wx.MINIMIZE_BOX|wx.TAB_TRAVERSAL )
		
		self.SetSizeHintsSz( wx.Size( 300,110 ), wx.DefaultSize )
		
		vSizer = wx.BoxSizer( wx.VERTICAL )
		
		hSizer_name = wx.BoxSizer( wx.HORIZONTAL )
		
		self.text_name = wx.StaticText( self, wx.ID_ANY, u"Name of result :", wx.DefaultPosition, wx.DefaultSize, 0 )
		self.text_name.Wrap( -1 )
		hSizer_name.Add( self.text_name, 0, wx.ALL|wx.ALIGN_CENTER_VERTICAL, 5 )
		
		self.textCtrl_name = wx.TextCtrl( self, wx.ID_ANY, u"result", wx.DefaultPosition, wx.DefaultSize, wx.TE_NO_VSCROLL|wx.TE_RIGHT )
		hSizer_name.Add( self.textCtrl_name, 0, wx.ALL|wx.ALIGN_CENTER_VERTICAL, 5 )
		
		choice_extChoices = [ u".gif", u".avi", u".mp4" ]
		self.choice_ext = wx.Choice( self, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, choice_extChoices, 0 )
		self.choice_ext.SetSelection( 0 )
		hSizer_name.Add( self.choice_ext, 0, wx.ALL, 5 )
		
		
		vSizer.Add( hSizer_name, 0, 0, 5 )
		
		hSizer_fps = wx.BoxSizer( wx.HORIZONTAL )
		
		self.text_pre_fps = wx.StaticText( self, wx.ID_ANY, u"Frame rate :", wx.DefaultPosition, wx.DefaultSize, 0 )
		self.text_pre_fps.Wrap( -1 )
		hSizer_fps.Add( self.text_pre_fps, 0, wx.ALL|wx.ALIGN_CENTER_VERTICAL, 5 )
		
		self.spinCtrl_fps = wx.SpinCtrl( self, wx.ID_ANY, wx.EmptyString, wx.DefaultPosition, wx.Size( 70,-1 ), wx.SP_ARROW_KEYS|wx.SP_WRAP, 1, 60, 15 )
		hSizer_fps.Add( self.spinCtrl_fps, 0, wx.ALL|wx.ALIGN_CENTER_VERTICAL, 5 )
		
		self.text_post_fps = wx.StaticText( self, wx.ID_ANY, u"fps", wx.DefaultPosition, wx.DefaultSize, 0 )
		self.text_post_fps.Wrap( -1 )
		hSizer_fps.Add( self.text_post_fps, 0, wx.ALL|wx.ALIGN_CENTER_VERTICAL, 5 )
		
		self.button = wx.Button( self, wx.ID_ANY, u"Build video", wx.DefaultPosition, wx.DefaultSize, 0 )
		hSizer_fps.Add( self.button, 0, wx.ALIGN_CENTER_VERTICAL|wx.ALL, 5 )
		
		
		vSizer.Add( hSizer_fps, 0, 0, 5 )
		
		
		self.SetSizer( vSizer )
		self.Layout()
		
		self.Centre( wx.BOTH )
		
		# Connect Events
		self.button.Bind( wx.EVT_BUTTON, self.build )
	
	def __del__( self ):
		pass
	
	
	# Virtual event handlers, overide them in your derived class
	def build( self, event ):
		event.Skip()
	

