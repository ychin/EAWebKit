import wx

# Character categories
kCharCategory_Lu        = 0     # Letter, Uppercase
kCharCategory_Ll        = 1     # Letter, Lowercase
kCharCategory_Lt        = 2     # Letter, Titlecase
kCharCategory_Lm        = 3     # Letter, Modifier
kCharCategory_Lo        = 4     # Letter, Other
kCharCategory_Mn        = 5     # Mark, Non-Spacing
kCharCategory_Mc        = 6     # Mark, Spacing Combining
kCharCategory_Me        = 7     # Mark, Enclosing
kCharCategory_Nd        = 8     # Number, Decimal
kCharCategory_Nl        = 9     # Number, Letter
kCharCategory_No        = 10    # Number, Other
kCharCategory_Pc        = 11    # Punctuation, Connector
kCharCategory_Pd        = 12    # Punctuation, Dash
kCharCategory_Ps        = 13    # Punctuation, Open
kCharCategory_Pe        = 14    # Punctuation, Close
kCharCategory_Pi        = 15    # Punctuation, Initial quote
kCharCategory_Pf        = 16    # Punctuation, Final quote
kCharCategory_Po        = 17    # Punctuation, Other
kCharCategory_Sm        = 18    # Symbol, Math
kCharCategory_Sc        = 19    # Symbol, Currency
kCharCategory_Sk        = 20    # Symbol, Modifier
kCharCategory_So        = 21    # Symbol, Other
kCharCategory_Zs        = 22    # Separator, Space
kCharCategory_Zl        = 23    # Separator, Line
kCharCategory_Zp        = 24    # Separator, Paragraph
kCharCategory_Cc        = 25    # Other, Control
kCharCategory_Cf        = 26    # Other, Format
kCharCategory_Cs        = 27    # Other, Surrogate
kCharCategory_Co        = 28    # Other, Private Use
kCharCategory_Cn        = 29    # Other, Not Assigned

categoryDict = { 
    'Lu' : kCharCategory_Lu, 'Ll' : kCharCategory_Ll, 'Lt' : kCharCategory_Lt,
    'Lm' : kCharCategory_Lm, 'Lo' : kCharCategory_Lo, 'Mn' : kCharCategory_Mn,
    'Mc' : kCharCategory_Mc, 'Me' : kCharCategory_Me, 'Nd' : kCharCategory_Nd,
    'Nl' : kCharCategory_Nl, 'No' : kCharCategory_No, 'Pc' : kCharCategory_Pc,
    'Pd' : kCharCategory_Pd, 'Ps' : kCharCategory_Ps, 'Pe' : kCharCategory_Pe,
    'Pi' : kCharCategory_Pi, 'Pf' : kCharCategory_Pf, 'Po' : kCharCategory_Po,
    'Sm' : kCharCategory_Sm, 'Sc' : kCharCategory_Sc, 'Sk' : kCharCategory_Sk,
    'So' : kCharCategory_So, 'Zs' : kCharCategory_Zs, 'Zl' : kCharCategory_Zl,
    'Zp' : kCharCategory_Zp, 'Cc' : kCharCategory_Cc, 'Cf' : kCharCategory_Cf,
    'Cs' : kCharCategory_Cs, 'Co' : kCharCategory_Co, 'Cn' : kCharCategory_Cn }
    
brushes = {}
categories = {}
    
class MainFrame( wx.Frame ):
    
    ID_MENU_EXIT = 600

    def __init__( self ):
        wx.Frame.__init__( self, None, -1, "Unicode Visualizer", size=( 512*2 + 8, 512*2 + 36 ) )
        
        wx.EVT_PAINT( self, self.OnPaint )

    def OnPaint( self, evt ):
        dc = wx.PaintDC( self )
        dc.SetPen( wx.TRANSPARENT_PEN )
       
        for y in range( 0, 256 ):
            for x in range( 0, 256 ):
                code = y * 256 + x
                if code in categories:
                    category = categories[ code ]
                
                    if category in brushes:
                        dc.SetBrush( brushes[ category ] );
                        dc.DrawRectangle( x * 4, y * 4, 4, 4 )
                
                
class UnicodeVisualizerApp( wx.App ):
    def OnInit( self ):
        frame = MainFrame()
        frame.Show( True )
        self.SetTopWindow( frame )
        
        brushes[ kCharCategory_Lu ] = wx.Brush( wx.Color( 255, 255,   0 ) )
        brushes[ kCharCategory_Ll ] = wx.Brush( wx.Color( 255, 128,   0 ) )
        brushes[ kCharCategory_Lt ] = wx.Brush( wx.Color( 128, 128,   0 ) )
        brushes[ kCharCategory_Lm ] = wx.Brush( wx.Color( 128, 255,   0 ) )
        brushes[ kCharCategory_Lo ] = wx.Brush( wx.Color( 128,  64,   0 ) )
        
        brushes[ kCharCategory_Nd ] = wx.Brush( wx.Color(   0, 255, 255 ) )
        brushes[ kCharCategory_Nl ] = wx.Brush( wx.Color(   0, 128, 255 ) )
        brushes[ kCharCategory_No ] = wx.Brush( wx.Color(   0, 128, 128 ) )
        
        brushes[ kCharCategory_Pc ] = wx.Brush( wx.Color( 128, 255, 128 ) )
        brushes[ kCharCategory_Pd ] = wx.Brush( wx.Color( 128, 128,  64 ) )
        brushes[ kCharCategory_Ps ] = wx.Brush( wx.Color(  64, 128, 128 ) )
        brushes[ kCharCategory_Pe ] = wx.Brush( wx.Color(  64, 255,  64 ) )
        brushes[ kCharCategory_Pi ] = wx.Brush( wx.Color(   0, 128, 128 ) )
        brushes[ kCharCategory_Pf ] = wx.Brush( wx.Color(   0, 128,  64 ) )
        brushes[ kCharCategory_Po ] = wx.Brush( wx.Color(  64, 255,   0 ) )

        brushes[ kCharCategory_Sm ] = wx.Brush( wx.Color(   0,   0,   0 ) )
        brushes[ kCharCategory_Sc ] = wx.Brush( wx.Color(  64,   0,  64 ) )
        brushes[ kCharCategory_Sk ] = wx.Brush( wx.Color(  64,  64,   0 ) )
        brushes[ kCharCategory_So ] = wx.Brush( wx.Color(   0,  64,  64 ) )

        brushes[ kCharCategory_Zs ] = wx.Brush( wx.Color( 255, 255, 255 ) )
        brushes[ kCharCategory_Zl ] = wx.Brush( wx.Color( 192, 192, 192 ) )
        brushes[ kCharCategory_Zp ] = wx.Brush( wx.Color( 128, 128, 128 ) )

        brushes[ kCharCategory_Mn ] = wx.Brush( wx.Color( 255,   0, 255 ) )
        brushes[ kCharCategory_Mc ] = wx.Brush( wx.Color( 255,   0, 192 ) )
        brushes[ kCharCategory_Me ] = wx.Brush( wx.Color( 255,   0, 128 ) )

        brushes[ kCharCategory_Cc ] = wx.Brush( wx.Color( 255,   0,   0 ) )
        brushes[ kCharCategory_Cf ] = wx.Brush( wx.Color( 255,  64,  64 ) )
        brushes[ kCharCategory_Cs ] = wx.Brush( wx.Color( 255, 140, 140 ) )
        brushes[ kCharCategory_Co ] = wx.Brush( wx.Color( 255, 128, 128 ) )
        brushes[ kCharCategory_Cn ] = wx.Brush( wx.Color( 255,   0,   0 ) )

        return True
        
# parse the unicode data
dbfile = open( "UnicodeData.txt", "rt" )
for line in dbfile:
    fields = line.split( ';' )
    
    code = int( fields[ 0 ], 16 )
    name = fields[ 1 ]
    category = categoryDict[ fields[ 2 ] ]
    combiningClass = fields[ 3 ]
    bidiClass = fields[ 4 ]
    decompType = fields[ 5 ]
    numericType = fields[ 6 ]
    
    categories[ code ] = category
    if name.endswith( ", First>" ):
        prevcode = code;
    if name.endswith( ", Last>" ):
        for code in range( prevcode, code ):
            categories[ code ] = category
    
    if code > 0xffff: break
    
app = UnicodeVisualizerApp( 0 )
app.MainLoop()

print "Done"
