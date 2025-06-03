using CapturaTestesGUI.Utils;
using System;
using System.Reflection;
using System.Xml.Linq;

namespace WinFormsApp1
{

    public class ChartControl : Control
    {

        public void CreatePoint(int index, double x, double y)
        {
            if (SeriesIndex.TryGetValue(index, out SerieChart serie))
            {
                CreatePoint(serie, x, y);
            }
        }

        public void CreatePoint(string name, double x, double y)
        {
            if (Series.TryGetValue(name, out SerieChart serie))
            {
                CreatePoint(serie, x, y);
            }
        }

        public void CreatePoint(SerieChart serie, double x, double y)
        {
            if (x < ValueMinX) ValueMinX = x;
            if (x > ValueMaxX) ValueMaxX = x;
            if (y < ValueMinY) ValueMinY = y;
            if (y > ValueMaxY) ValueMaxY = y;

            if (ValueMinX > 0 && ValueMaxX > 0) ValueMinX = 0;
            if (ValueMinX < 0 && ValueMaxX < 0) ValueMaxX = 0;
            if (ValueMinY > 0 && ValueMaxY > 0) ValueMinY = 0;
            if (ValueMinY < 0 && ValueMaxY < 0) ValueMaxY = 0;

            RangeX = ValueMaxX - ValueMinX;
            RangeY = ValueMaxY - ValueMinY;


            if (x < serie.ValueMinX) serie.ValueMinX = x;
            if (x > serie.ValueMaxX) serie.ValueMaxX = x;
            if (y < serie.ValueMinY) serie.ValueMinY = y;
            if (y > serie.ValueMaxY) serie.ValueMaxY = y;

            if (serie.ValueMinX > 0 && serie.ValueMaxX > 0) serie.ValueMinX = 0;
            if (serie.ValueMinX < 0 && serie.ValueMaxX < 0) serie.ValueMaxX = 0;
            if (serie.ValueMinY > 0 && serie.ValueMaxY > 0) serie.ValueMinY = 0;
            if (serie.ValueMinY < 0 && serie.ValueMaxY < 0) serie.ValueMaxY = 0;

            serie.RangeX = serie.ValueMaxX - serie.ValueMinX;
            serie.RangeY = serie.ValueMaxY - serie.ValueMinY;


            lock (ChartLock)
            {
                serie.Points.Add(new PointChart() { X = x, Y = y, Index = (ulong)serie.Points.Count });
            }

            Invalidate();
        }

        public void CreateSerie(int index, SerieLineType line_type = SerieLineType.Line, Color? color = null, float thickness = 1)
        {
            var serie = new SerieChart() { Index = index, LineType = line_type, Color = color, Thickness = thickness };

            if (!color.HasValue)
            {
                serie.Color = ColorGenerator.Create();
            }

            SeriesIndex.Add(index, serie);
            Series.Add(index.ToString(), serie);
        }

        public void CreateSerie(string name, SerieLineType line_type = SerieLineType.Line, Color? color = null, float thickness = 1)
        {
            var serie = new SerieChart() { Name = name, LineType = line_type, Color = color, Thickness = thickness };

            if (!color.HasValue)
            {
                serie.Color = ColorGenerator.Create();
            }

            Series.Add(name, serie);
            SeriesIndex.Add(Series.Count, serie);
        }

        public SerieChart GetSerie(int index)
        {
            if (SeriesIndex.TryGetValue(index, out SerieChart serie))
            {
                return serie;
            }
            return null;
        }
        public SerieChart GetSerie(string name)
        {
            if (Series.TryGetValue(name, out SerieChart serie))
            {
                return serie;
            }
            return null;
        }

        protected override void OnPaint(PaintEventArgs e)
        {
            base.OnPaint(e);
            var g = e.Graphics;
            g.SmoothingMode = System.Drawing.Drawing2D.SmoothingMode.AntiAlias;

            var info = PrepareAxis();
            PlotGrid(info, g);

            var displayX   = (RangeX > MaxX) ? MaxX : RangeX;
            var propo_x    = info.AxisX.ScreenSize / displayX;
            var propo_y    = info.AxisY.ScreenSize / RangeY;

            var left_x  = ValueMinX;
            var right_x = ValueMaxX;
            if(RangeX > MaxX)
            {
                left_x = ValueMaxX - MaxX;
            }

            foreach (var serie in Series.Values)
            {
                var visibles = new PointChart[0];
                lock (ChartLock) 
                {
                    visibles = serie.Points.Where(w => w.X >= left_x && w.X <= right_x).ToArray();

                    if (RemoveNonVisible)
                    {
                        var outside = serie.Points.Where(w => !(w.X >= left_x && w.X <= right_x));
                        serie.Points.RemoveRange(0, serie.Points.Count - outside.Count());
                    }
                }

                var last = serie.Points.LastOrDefault();

                if (visibles.Length > 0)
                {
                    var first = visibles[0];

                    int ix = 0;
                    double prev_x = 0, prev_y = 0;
                    if(first.Index > 0)
                    {
                        PointChart before = null;
                        lock (ChartLock)
                        {
                            before = serie.Points[(int)first.Index - 1];
                        }

                        if (before.X < left_x)  // Ponto anterior está fora da tela 
                        {
                            // Faz interpolação linear para encontrar ponto na borda
                            double dx = first.X - before.X;
                            double dy = first.Y - before.Y;
                            double ratio = (left_x - before.X) / dx;

                            double interp_x = left_x;
                            double interp_y = before.Y + ratio * dy;

                            prev_x = info.AxisX.ScreenStart;
                            prev_y = info.AxisY.ScreenEnd - ((interp_y - ValueMinY) * propo_y);
                        }
                        else
                        {
                            prev_x = info.AxisX.ScreenStart + ((before.X - left_x) * propo_x);
                            prev_y = info.AxisY.ScreenEnd - ((before.Y - ValueMinY) * propo_y);
                        }
                    }
                    else
                    {
                        prev_x = info.AxisX.ScreenStart + ((first.X - left_x) * propo_x);
                        prev_y = info.AxisY.ScreenEnd - ((first.Y - ValueMinY) * propo_y);
                        ix = 1;
                    }

                    var pen = new Pen(serie.Color.Value, serie.Thickness);

                    while (ix < visibles.Length)
                    {
                        var point = visibles[ix];
                        var pos_x = info.AxisX.ScreenStart + ((point.X - left_x) * propo_x);
                        var pos_y = info.AxisY.ScreenEnd   - ((point.Y - ValueMinY) * propo_y);

                        var delta_x = pos_x - prev_x;
                        var delta_y = pos_y - prev_y;

                        if (delta_x <= -2 || delta_x >= 2 || delta_y <= -2 || delta_y >= 2)// Nao renderiza se diferenca pequena.
                        {
                            g.DrawLine(pen, (float)prev_x, (float)prev_y, (float)pos_x, (float)pos_y);

                            prev_x = pos_x;
                            prev_y = pos_y;
                        }
                       
                        ix++;
                    }

                    pen.Dispose();
                }
            }
        }


        private void PlotGrid(CoordInfo info, Graphics g)
        {
            var fz = Graphics.FromImage(new Bitmap(1, 1)).MeasureString("Amg", DEFAULT_FONT);
            var op = fz.Height / 2.2f;
            var fa = Graphics.FromImage(new Bitmap(1, 1)).MeasureString(FormatX, DEFAULT_FONT);
            var oq = fz.Width / 2.6f;
            var line_pen = new Pen(Color.FromArgb(240, 240, 240), 1);

            int i = 0;
            while (i < Columns)
            {
                var pos   = (float)(info.AxisX.ScreenStart + (i * info.AxisX.ScreenStep));
                var value = info.AxisX.ValueStart + (info.AxisX.ValueStep * i);

                g.DrawLine(line_pen, pos, (float)info.AxisY.ScreenStart, pos, (float)info.AxisY.ScreenEnd);

                if (ColumnValueTop)
                {
                    g.DrawString(value.ToString(FormatX), DEFAULT_FONT, new SolidBrush(DEFAULT_FONT_COLOR), (float)pos - oq, (float)(MarginY + op));
                }
                else
                {
                    g.DrawString(value.ToString(FormatX), DEFAULT_FONT, new SolidBrush(DEFAULT_FONT_COLOR), (float)pos - oq, (float)(Height - MarginY - fz.Height));
                }
                i++;
            }

            i = 0;
            while (i < Lines)
            {
                var pos = (float)(info.AxisY.ScreenStart + (i * info.AxisY.ScreenStep));
                var value = ValueMaxY - (info.AxisY.ValueStep * i);

                g.DrawLine(line_pen, (float)info.AxisX.ScreenStart, pos, (float)info.AxisX.ScreenEnd, pos);

                if (LineValueRight)
                {
                    g.DrawString(value.ToString(FormatX), DEFAULT_FONT, new SolidBrush(DEFAULT_FONT_COLOR), (float)(Width - MarginX), (float)(pos - op));
                }
                else
                {
                    g.DrawString(value.ToString(FormatX), DEFAULT_FONT, new SolidBrush(DEFAULT_FONT_COLOR), (float)MarginX, (float)(pos - op));
                }
                i++;
            }
        }


        private CoordInfo PrepareAxis()
        {
            CoordInfo info = new CoordInfo();

            // quanto de X cabe na área visível
            var displayRangeX      = Math.Min(RangeX, MaxX);

            // origem em X: se rolou, começa em ValueMaxX-MaxX
            info.AxisX.ValueStart  = (RangeX > MaxX) ? (ValueMaxX - MaxX) : ValueMinX;

            // passo de cada coluna sempre sobre o displayRangeX
            info.AxisX.ValueStep   = displayRangeX / (Columns - 1);

            info.AxisY.ValueStep   = RangeY / (Lines - 1);

            info.AxisX.ScreenStart = LineValueRight ? MarginX : TextAreaX + MarginX;
            info.AxisX.ScreenEnd   = Width - (LineValueRight ? TextAreaX + MarginX : MarginX);
            info.AxisX.ScreenSize  = info.AxisX.ScreenEnd - info.AxisX.ScreenStart;
            info.AxisX.ScreenStep  = info.AxisX.ScreenSize / (Columns - 1);

            info.AxisY.ScreenStart = ColumnValueTop ? TextAreaY + MarginY : MarginY;
            info.AxisY.ScreenEnd   = Height - (ColumnValueTop ? MarginY : TextAreaY + MarginY);
            info.AxisY.ScreenSize  = info.AxisY.ScreenEnd - info.AxisY.ScreenStart;
            info.AxisY.ScreenStep  = info.AxisY.ScreenSize / (Lines - 1);

            return info;
        }


        protected override void OnResize(EventArgs e)
        {
            base.OnResize(e);
            Invalidate();
        }


        public bool RemoveNonVisible;

        private double RangeX;
        private double RangeY;
        private double ValueMinX;
        private double ValueMaxX;
        private double ValueMinY;
        private double ValueMaxY;
        private double MaxX;
        private double Lines;
        private double Columns;
        private double MarginX;
        private double MarginY;
        private double TextAreaX;
        private double TextAreaY;
        private bool   LineValueRight = false;
        private bool   ColumnValueTop = false;
        private ChartControlLock ChartLock;
        private Dictionary<string, SerieChart> Series;
        private Dictionary<int, SerieChart> SeriesIndex;


        public Font   DEFAULT_FONT = new Font("Arial", 8);
        public Color  DEFAULT_FONT_COLOR = Color.FromArgb(160, 180, 200);
        public string FormatX = "0.00";
        public string FormatY = "0.00";
        

        public ChartControl()
        {
            DoubleBuffered   = true;
            RemoveNonVisible = false;

            Series      = new Dictionary<string, SerieChart>();
            SeriesIndex = new Dictionary<int, SerieChart>();

            MaxX   = 10;

            Lines   = 11;
            Columns = 10;
            MarginX = 20;
            MarginY = 20;

            TextAreaX = 40;
            TextAreaY = 20;

            BackColor = Color.White;

            ChartLock = new ChartControlLock();
        }

    }


    internal class ChartControlLock { }


    internal struct AxisPoint
    {
        public double ScreenStep;
        public double ScreenStart;
        public double ScreenEnd;
        public double ScreenSize;
        public double ValueStart;
        public double ValueStep;
    }
    internal struct CoordInfo
    {
        public AxisPoint AxisX;
        public AxisPoint AxisY;
    }
}
