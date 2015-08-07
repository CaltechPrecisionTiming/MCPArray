
class Centers:
    def __init__(self, x, y, # sx, sy, 
                 realx, realy):
        self.x = x
        self.y = y
        # self.sx = sx
        # self.sy = sy
        self.realx = realx
        self.realy = realy
    def error(self, center2):
        return (self.x - center2.x) - (self.realx - center2.realx),\
            (self.y - center2.y) - (self.realy - center2.realy)

def getpoints(clist):
    points = []
    clist2 = clist[1:]
    for c1 in clist:
        for c2 in clist2:
            if c1 != c2:
                points.append(tuple(c1.error(c2)))
                #print c1.error(c2)
        if len(clist2): clist2.pop(0)
    return points

if __name__ == "__main__":
    from ROOT import gROOT, TCanvas, TH1F, TF1, gStyle, TLatex
    gROOT.Reset()

    clistA = [
        Centers(9.34121, 9.31852, 12, 22), # 21
        Centers(9.2973 , 8.59611, 12, 21), # 22
        Centers(9.27621, 9.37349, 12, 22), # 23
        Centers(9.36325, 8.69264, 12, 21), # 24
        Centers(9.28944, 8.00009, 12, 20), # 25
        Centers(9.1551 , 7.23947, 12, 19), # 26
        Centers(9.10545,   6.628, 12, 18), # 27
        Centers(9.09252, 5.82378, 12, 17), # 28
        Centers(8.94966, 5.53106, 12, 16), # 29
        Centers(9.40006, 9.39145, 12, 22), # 30
        Centers(9.30713, 8.61396, 12, 21), # 31
        Centers(9.22575, 8.03422, 12, 20), # 32
        Centers(9.26738, 7.33175, 12, 19), # 33
        Centers(9.06553, 6.5906 , 12, 18), # 34
        Centers(8.98611, 6.06684, 12, 17), # 35
        Centers(8.92167, 5.07548, 12, 16), # 36
        Centers(9.36273, 11.0906, 12, 24), # 37
        Centers(9.18288, 12.6198, 12, 26), # 38
        Centers(9.12667, 13.3777, 12, 27), # 39
        # Centers(10.2276, 9.46013, 10, 22), # 40
        # Centers(11.4684, 9.56123,  8, 22), # 41
        # Centers(12.5611, 9.51713,  6, 22), # 42
        ]

    clistI = [
        Centers(9.27796, 9.51198, 12, 22), # 21
        Centers(9.29283, 8.53305, 12, 21), # 22
        Centers(9.20545, 9.63472, 12, 22), # 23
        Centers(9.3435 , 8.75356, 12, 21), # 24
        Centers(9.23891, 7.82817, 12, 20), # 25
        Centers(9.10313, 6.97577, 12, 19), # 26
        Centers(9.0287 , 6.29648, 12, 18), # 27
        Centers(8.90299, 5.56551, 12, 17), # 28
        Centers(8.86388, 4.49061, 12, 16), # 29
        Centers(9.36233, 9.63089, 12, 22), # 30
        Centers(9.25   , 8.6491 , 12, 21), # 31
        Centers(9.15665, 7.77052, 12, 20), # 32
        Centers(9.21107, 6.94159, 12, 19), # 33
        Centers(8.99333, 6.12369, 12, 18), # 34
        Centers(8.91897, 5.53766, 12, 17), # 35
        Centers(8.77638, 4.6662 , 12, 16), # 36
        Centers(9.25372, 11.3672, 12, 24), # 37
        Centers(9.09835, 12.8532, 12, 26), # 38
        Centers(9.05256, 13.5172, 12, 27), # 39
        # Centers(10.3491, 9.67175, 10, 22), # 40
        # Centers(11.6273, 9.60224,  8, 22), # 41
        # Centers(12.7906, 9.56366,  6, 22), # 42
        ]

    pointsA = getpoints(clistA)
    pointsI = getpoints(clistI)

    histogramAx = TH1F.TH1F('AmpResX', '; X residuals [mm]; Entries / 0.4 mm', 25, -5, 5)
    histogramAy = TH1F.TH1F('AmpResY', '; Y residuals [mm]; Entries / 0.4 mm', 25, -5, 5)
    histogramIx = TH1F.TH1F('IntResX', '; X residuals [mm]; Entries / 0.4 mm', 25, -5, 5)
    histogramIy = TH1F.TH1F('IntResY', '; Y residuals [mm]; Entries / 0.2 mm', 40, -4, 4)

    for p in pointsA:
        histogramAx.Fill(p[0])
        histogramAy.Fill(p[1])

    for p in pointsI:
        histogramIx.Fill(p[0])
        histogramIy.Fill(p[1])

    c1 = TCanvas('c1', 'c1', 200, 10, 800, 700)
    c1.SetHighLightColor(2);
    c1.SetFillColor(0);
    c1.SetBorderMode(0);
    c1.SetBorderSize(2);
    c1.SetLeftMargin(0.12);
    c1.SetRightMargin(0.05);
    c1.SetTopMargin(0.07);
    c1.SetBottomMargin(0.12);
    c1.SetFrameBorderMode(0);
    c1.SetFrameBorderMode(0);

    gStyle.SetOptFit(0)
    gStyle.SetOptStat(0)

    tex = TLatex.TLatex()
    tex.SetTextSize(0.06);
    gaussian1 = TF1.TF1("spaceres", "gaus", -1.75, 1.75);
    # gaussian2 = TF1.TF1("spaceres", "gaus", -2, 2);
    # gaussian3 = TF1.TF1("spaceres", "gaus", -0.75, 0.75);

    # gaussian3.SetParameters(histogramAx.GetEntries()/3, histogramAx.GetMean(), histogramAx.GetRMS())
    # histogramAx.Fit(gaussian3, "MQR")
    # histogramAx.Draw()

    # c1.SaveAs("outputs/residualsAx.gif")
    # c1.SaveAs("outputs/residualsAx.pdf")

    # gaussian2.SetParameters(histogramAy.GetEntries()/3, histogramAy.GetMean(), histogramAy.GetRMS())
    # histogramAy.Fit(gaussian2, "LMQR")
    # histogramAy.Draw()
    # c1.SaveAs("outputs/residualsAy.gif")
    # c1.SaveAs("outputs/residualsAy.pdf")

    # gaussian3.SetParameters(histogramIx.GetEntries()/3, histogramIx.GetMean(), histogramIx.GetRMS())
    # histogramIx.Fit(gaussian3, "MQR")
    # histogramIx.Draw()
    # c1.SaveAs("outputs/residualsIx.gif")
    # c1.SaveAs("outputs/residualsIx.pdf")

    gaussian1.SetParameters(histogramIx.GetEntries()/3, histogramIx.GetMean(), histogramIx.GetRMS())
    histogramIy.Fit(gaussian1, "LMQR")
    histogramIy.GetXaxis().SetTitleSize(0.06);
    histogramIy.GetYaxis().SetTitleSize(0.06);
    histogramIy.GetXaxis().SetTitleOffset(.8);
    histogramIy.GetYaxis().SetTitleOffset(.8);

    histogramIy.Draw()
    tex.DrawLatexNDC(.65, .80, "#sigma: %2.1f mm" % gaussian1.GetParameter(2))
    c1.SaveAs("outputs/residualsIy.gif")
    c1.SaveAs("outputs/residualsIy.pdf")
 
