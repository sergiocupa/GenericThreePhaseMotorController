using CapturaTestesGUI.Utils;

namespace WinFormsApp1
{

    public class ChartControl : Control
    {

        public void CreatePoint(string name, double x, double y)
        {
            if(Series.TryGetValue(name, out SerieChart serie))
            {
                if (x < ValueMinX) ValueMinX = x;
                if (x > ValueMaxX) ValueMaxX = x;
                if (y < ValueMinY) ValueMinY = y;
                if (y > ValueMaxY) ValueMaxY = y;

                RangeX = ValueMaxX - ValueMinX;
                RangeY = ValueMaxY - ValueMinY;

                lock(ChartLock)
                {
                    serie.Points.Add(new PointChart() { X = x, Y = y });
                }
                
                Invalidate();
            }
        }

        public void CreateSerie(string name, SerieLineType line_type = SerieLineType.Line, Color? color = null, float thickness = 1)
        {
            var serie = new SerieChart() { Name = name, LineType = line_type, Color = color, Thickness = thickness };

            if (!color.HasValue)
            {
                serie.Color = ColorGenerator.Create();
            }

            Series.Add(name, serie);
        }

        protected override void OnPaint(PaintEventArgs e)
        {
            base.OnPaint(e);
            var g = e.Graphics;

            var info = PrepareAxis();
            PlotGrid(info, g);

            foreach (var serie in Series.Values)
            {
                var left_x  = ValueMinX;
                var right_x = ValueMaxX;

                if (RangeX > MaxX)
                {
                    left_x = ValueMaxX - MaxX;
                }

                var visibles = new PointChart[0];
                lock (ChartLock) 
                {
                    visibles = serie.Points.Where(w => (w.X >= left_x && w.X <= right_x) && (w.Y >= ValueMinY && w.Y <= ValueMaxY)).ToArray();
                }

                var last = serie.Points.LastOrDefault();

                if (visibles.Length > 1)
                {
                    var x_min   = visibles.Min(m => m.X);
                    var x_max   = visibles.Max(m => m.X);
                    var y_min   = visibles.Min(m => m.Y);
                    var y_max   = visibles.Max(m => m.Y);
                    var x_range = x_max - (x_min < 0 ? x_min : 0);
                    var y_range = y_max - (y_min < 0 ? y_min : 0);

                    var propo_w = info.AxisX.ScreenSize / x_range;
                    var propo_h = info.AxisY.ScreenSize / y_range;

                    var first   = visibles[0];
                    var prev_x  = info.AxisX.ScreenStart + ((first.X - (x_min < 0 ? x_min : 0)) * propo_w);
                    var prev_y  = info.AxisY.ScreenEnd   - ((first.Y - (y_min < 0 ? y_min : 0)) * propo_h);

                    var pen = new Pen(serie.Color.Value, serie.Thickness);

                    int ix = 1;
                    while (ix < visibles.Length)
                    {
                        var point = visibles[ix];
                        var pos_x = info.AxisX.ScreenStart + ((point.X - (x_min < 0 ? x_min : 0)) * propo_w);
                        var pos_y = info.AxisY.ScreenEnd   - ((point.Y - (y_min < 0 ? y_min : 0)) * propo_h);

                        g.DrawLine(pen, (float)prev_x, (float)prev_y, (float)pos_x, (float)pos_y);

                        prev_x = pos_x;
                        prev_y = pos_y;
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
                var pos = (float)(info.AxisX.ScreenStart + (i * info.AxisX.ScreenStep));
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

            if (RangeX > MaxX)
            {
                var rx = RangeX - MaxX;
                info.AxisX.ValueStart = ValueMinX + rx;
                info.AxisX.ValueStep = rx / (Columns - 1);
            }
            else
            {
                info.AxisX.ValueStart = ValueMinX;
                info.AxisX.ValueStep = RangeX / (Columns - 1);
            }

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


        public double RangeX;
        public double RangeY;
        public double ValueMinX;
        public double ValueMaxX;
        public double ValueMinY;
        public double ValueMaxY;


        private double MaxX;
        private double Lines;
        private double Columns;
        private double MarginX;
        private double MarginY;
        private double TextAreaX;
        private double TextAreaY;

        private bool LineValueRight = false;
        private bool ColumnValueTop = false;

        public Font DEFAULT_FONT = new Font("Arial", 8);
        public Color DEFAULT_FONT_COLOR = Color.FromArgb(160, 180, 200);
        public string FormatX = "0.00";
        public string FormatY = "0.00";
        ChartControlLock ChartLock;
        private Dictionary<string, SerieChart> Series;

        public ChartControl()
        {
            DoubleBuffered = true;

            Series = new Dictionary<string, SerieChart>();

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
