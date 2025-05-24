using CapturaTestesGUI.Utils;
using System.Windows.Forms.DataVisualization.Charting;


namespace WinFormsApp1
{
    public partial class ValueChart : Control
    {

        public void Append(double x, double y, string? serie_name = null, int _serie_id = 0, bool is_update = false, Color? color = null, bool layout_control = true)
        {
            if (InvokeRequired)
            {
                Invoke(() => { AppendInvoke(x, y, serie_name, _serie_id, is_update, color, layout_control); });
            }
            else
            {
                AppendInvoke(x, y, serie_name, _serie_id, is_update, color, layout_control);
            }
        }


        private void AppendInvoke(double x, double y, string? serie_name, int _serie_id, bool is_update, Color? color, bool layout_control)
        {
            try
            {
                if (layout_control) Chart.SuspendLayout();

                Series serie = null;

                if (!string.IsNullOrEmpty(serie_name))
                {
                    serie = Chart.Series.Where(w => w.Name == serie_name).FirstOrDefault();
                }
                if (serie == null && _serie_id >= 0 && _serie_id < Chart.Series.Count)
                {
                    serie = Chart.Series[_serie_id];
                }

                if (serie != null)
                {
                    UpdateY(y);

                    if (is_update)
                    {
                        var pt = serie.Points.LastOrDefault();
                        if (pt != null)
                        {
                            var pp = (CandlePoint)pt;
                            pp.XValue = x;
                            pp.InputX = x;
                            pp.YValues = new double[] { y };
                        }
                    }
                    else
                    {
                        var dp = new CandlePoint() { InputX = x, XValue = x, YValues = new double[] { y } };
                        if (color.HasValue)
                        {
                            dp.Color = color.Value;
                        }

                        serie.Points.Add(dp);
                    }

                    UpdateRangeX(x, serie);
                }

                if (layout_control) Chart.ResumeLayout();
            }
            catch (Exception ex)
            {
                Console.Error.WriteLine(ex.ToString());
            }
        }


        private void UpdateRangeX(double x, Series serie)
        {
            if (serie.Points.Count >= 2)
            {
                var first = (CandlePoint)serie.Points.FirstOrDefault();
                var last = (CandlePoint)serie.Points.LastOrDefault();
                var dif =  last.InputX - first.InputX;

                if (dif > SizeX)
                {
                    // Plota deslocando
                    var a = x - SizeX;

                    if (Step.HasValue)
                    {
                        a = a - Step.Value;
                    }

                    Area.AxisX.Minimum = a +  OffsetX;
                    Area.AxisX.Maximum = x +  OffsetX;
                }
                else
                {
                    // Plota ate fim da tela
                    var a = first.InputX;
                    if (Step.HasValue)
                    {
                        a = a - Step.Value;
                    }
                    Area.AxisX.Minimum = a + OffsetX;

                    var m = first.InputX + SizeX;
                    Area.AxisX.Maximum = m + OffsetX;
                }
            }
            else
            {
                var first = (CandlePoint)serie.Points.FirstOrDefault();
                if (first != null)
                {
                    // Plota ate fim da tela
                    var a = first.InputX;
                    if (Step.HasValue)
                    {
                        a = a - Step.Value;
                    }
                    Area.AxisX.Minimum = a + OffsetX ;

                    var m = first.InputX + SizeX + OffsetX;
                    Area.AxisX.Maximum = m + OffsetX;
                }
            }
        }

        public void ResetY(double min, double max, bool layout_control = true)
        {
            if (layout_control) Chart.SuspendLayout();

            if (Chart.InvokeRequired)
            {
                Chart.Invoke(() =>
                {
                    Chart.ChartAreas[0].AxisY2.Minimum = min;
                    Chart.ChartAreas[0].AxisY2.Maximum = max;
                });
            }
            else
            {
                Chart.ChartAreas[0].AxisY2.Minimum = min;
                Chart.ChartAreas[0].AxisY2.Maximum = max;
            }

            if (layout_control) Chart.ResumeLayout();
        }


        private void UpdateY(double value)
        {
            if (value > Chart.ChartAreas[0].AxisY2.Maximum)
            {
                Chart.ChartAreas[0].AxisY2.Maximum = value;
            }
            if (value < Chart.ChartAreas[0].AxisY2.Minimum)
            {
                Chart.ChartAreas[0].AxisY2.Minimum = value;
            }
        }


        public void AddSerie(string name, SeriesChartType type = SeriesChartType.Line, Color? color = null, bool layout_control = true)
        {
            if (Chart.InvokeRequired)
            {
                Chart.Invoke(() => { _AddSerie(name, type, color, layout_control); });
            }
            else
            {
                _AddSerie(name, type, color, layout_control);
            }
        }

        private void _AddSerie(string name, SeriesChartType type, Color? color, bool layout_control)
        {
            var ex = Chart.Series.Where(w => w.Name == name).FirstOrDefault();
            if (ex == null)
            {
                if (layout_control) Chart.SuspendLayout();

                var s = new Series();

                s.Name = name;
                s.ChartType = type;
                s.BorderWidth = 1;
                s.YAxisType = AxisType.Secondary;

                s.IsXValueIndexed = false;

                if (type == SeriesChartType.Candlestick)
                {
                    // Configuração do estilo de vela
                    //s["OpenCloseStyle"] = "Triangle"; // Forma dos marcadores de abertura/fechamento
                    s["OpenCloseStyle"] = "Candlestick"; // Forma dos marcadores de abertura/fechamento
                    s["ShowOpenClose"] = "Both"; // Mostra abertura e fechamento
                    s["PointWidth"] = "0.6"; // Largura das velas

                    // Cores de alta e baixa
                    s["PriceUpColor"] = "#78C8FF"; // Alta
                    s["PriceDownColor"] = "#FFBB60"; // Baixa
                }
                else
                {
                    s.Color = color.HasValue ? color.Value : ColorGenerator.Create();

                    if (type == SeriesChartType.Point)
                    {
                        s.MarkerStyle = MarkerStyle.Circle;
                        s.MarkerSize = 6;
                    }
                }
                Chart.Series.Add(s);

                if (layout_control) Chart.ResumeLayout();
            }
        }


        private void Chart_MouseWheel(object? sender, MouseEventArgs e)
        {
            try
            {
                Area.RecalculateAxesScale();


                if ((Control.ModifierKeys & Keys.Control) == Keys.Control)// Eixo X
                {
                    var difX = Area.AxisX.Maximum - Area.AxisX.Minimum; // Calcula o intervalo de tempo atual
                    var dif6 = difX / 6.0; 
                    var dif2 = difX / 2.0;

                    if (e.Delta > 0) // Zoom in no eixo X (reduz intervalo)
                    {
                        var novoMinX = Area.AxisX.Minimum + dif6;
                        var novoMaxX = Area.AxisX.Maximum - dif6;

                        if (novoMinX < novoMaxX)
                        {
                            SizeX = novoMaxX - novoMinX;

                            // Converte de volta para OADate antes de atribuir
                            Area.AxisX.Minimum = novoMinX;
                            Area.AxisX.Maximum = novoMaxX;
                        }
                    }
                    else if (e.Delta < 0)
                    {
                        var novoMinX = Area.AxisX.Minimum - dif2;
                        var novoMaxX = Area.AxisX.Maximum + dif2;

                        SizeX = novoMaxX - novoMinX;

                        // Converte de volta para OADate antes de atribuir
                        Area.AxisX.Minimum = novoMinX;
                        Area.AxisX.Maximum = novoMaxX;
                    }
                }
                else// Eixo Y
                {
                    if (e.Delta > 0) // Zoom in (reduz intervalo)
                    {
                        var dif = (Area.AxisY2.Maximum - Area.AxisY2.Minimum) / 6.0;

                        double novoMin = Area.AxisY2.Minimum + dif;
                        double novoMax = Area.AxisY2.Maximum - dif;

                        if (novoMin < novoMax)
                        {
                            Area.AxisY2.Minimum = novoMin;
                            Area.AxisY2.Maximum = novoMax;
                        }
                    }
                    else if (e.Delta < 0) // Zoom out (aumenta intervalo)
                    {
                        var dif = (Area.AxisY2.Maximum - Area.AxisY2.Minimum) / 2.0;

                        Area.AxisY2.Maximum += dif;
                        Area.AxisY2.Minimum -= dif;
                    }
                }

                Area.RecalculateAxesScale();
            }
            catch (Exception ex)
            {
                Console.Error.WriteLine(ex.ToString());
            }
        }

        // Testar todas as series para ver qual mais proximo
        // Ignorar se: mouse movimentando
        private DataPoint? GetNearestCandle(Chart chart, int mouseX, int mouseY, int serie_index, ref double min_distance)
        {
            DataPoint? nearestPoint = null;

            double xValue = Area.AxisX.PixelPositionToValue(mouseX);
            double yValue = Area.AxisY2.PixelPositionToValue(mouseY);

            Series series = chart.Series[serie_index];
            double distance = double.MaxValue;
            bool setted = false;

            if (series.ChartType == SeriesChartType.Candlestick)
            {
                foreach (DataPoint candle in series.Points)
                {
                    double xDistance = Math.Abs(candle.XValue - xValue);

                    double high = candle.YValues[1];
                    double low = candle.YValues[0];

                    if (yValue >= low && yValue <= high && xDistance < distance)
                    {
                        distance = xDistance;
                        nearestPoint = candle;
                        setted = true;
                    }
                }
            }
            else
            {
                foreach (DataPoint point in series.Points)
                {
                    // Calcula a distância entre o ponto clicado e cada ponto da série
                    double d = Math.Sqrt(Math.Pow(point.XValue - xValue, 2) + Math.Pow(point.YValues[0] - yValue, 2));

                    if (d < distance)
                    {
                        distance = d;
                        nearestPoint = point;
                        setted = true;
                    }
                }
            }

            if (setted) min_distance = distance;

            return nearestPoint;
        }


        private void ChartClick(int x, int y)
        {
            var list = new List<DataPointDistance>();
            double distance = 0;

            int ix = 0;
            while (ix < Chart.Series.Count)
            {
                DataPoint? dp = GetNearestCandle(Chart, x, y, ix, ref distance);
                if (dp != null)
                {
                    var ob = new DataPointDistance() { Point = dp, Distance = distance, SerieIndex = ix };
                    list.Add(ob);
                }
                ix++;
            }

            var ol = list.OrderBy(s => s.Distance).FirstOrDefault();
            if (ol != null)
            {
                var se = Chart.Series[ol.SerieIndex];

                if (se.ChartType == SeriesChartType.Candlestick)
                {
                    if (SelectedCandle != null) SelectedCandle((CandlePoint)ol.Point);
                }
                else
                {
                    //MessageBox.Show($"{se.Name}:\nHora: {time:HH:mm:ss}\nValor: {ol.Point.YValues[0]:F2}");
                }
            }
        }


        private void Chart_MouseMove(object? sender, MouseEventArgs e)
        {
            double novoMinX = 0;
            double novoMaxX = 0;

            try
            {
                if (IsDragging)
                {
                    Area.RecalculateAxesScale();

                    // Y
                    double rangeY = Area.AxisY2.Maximum - Area.AxisY2.Minimum;
                    double moveFactorY = (e.Y - LastMouseY) * (rangeY / 500.0);
                    Area.AxisY2.Minimum += moveFactorY;
                    Area.AxisY2.Maximum += moveFactorY;
                    LastMouseY = e.Y;

                    if (!Step.HasValue) Step = 1;

                    var PixelsPerStep = (double)Width / ((double)SizeX / (double)Step.Value);
                    var Displacement = e.X - LastMouseX;

                    if (Displacement >= PixelsPerStep)
                    {
                        novoMinX = Area.AxisX.Minimum - Step.Value;
                        LastMouseX = e.X;

                        Area.AxisX.Minimum = novoMinX;
                        novoMaxX = novoMinX + SizeX;
                        Area.AxisX.Maximum = novoMaxX;
                    }
                    else if (Displacement <= (-PixelsPerStep))
                    {
                        novoMinX = Area.AxisX.Minimum + Step.Value;
                        LastMouseX = e.X;

                        Area.AxisX.Minimum = novoMinX;
                        novoMaxX = novoMinX + SizeX;
                        Area.AxisX.Maximum = novoMaxX;
                    }

                    Area.RecalculateAxesScale();
                }
            }
            catch (Exception ex)
            {
                Console.Error.WriteLine(ex.ToString());
            }
        }

        private void Chart_MouseDown(object? sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left)
            {
                IsDragging = true;
                LastMouseY = e.Y;
                LastMouseX = e.X;
                StartMouseX = e.X;
                StartMouseY = e.Y;
            }
        }

        private void Chart_MouseUp(object? sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left)
            {
                IsDragging = false;

                var md = (Math.Abs(StartMouseX - e.X) + Math.Abs(StartMouseY - e.Y)) / 2.0;
                if (md < 2)
                {
                    ChartClick(e.X, e.Y);
                }
            }
        }



        public void InitAxisX()
        {
            Area.AxisX.Maximum = SizeX;
            Area.AxisX.Minimum = 0;
        }

        public void Init()
        {
            SizeX   = 1;
            OffsetX = 0;

            Chart = new Chart() { BackColor = Color.FromArgb(70, 70, 80), Dock = DockStyle.Fill };
            Controls.Add(Chart);

            Chart.MouseWheel += Chart_MouseWheel;
            Chart.MouseUp += Chart_MouseUp;
            Chart.MouseDown += Chart_MouseDown;
            Chart.MouseMove += Chart_MouseMove;

            Area = new ChartArea() { BackColor = Color.FromArgb(70, 70, 80) };
            Area.Name = "CandleArea";
            Chart.ChartAreas.Add(Area);

            Area.Position = new ElementPosition(0, 1, 101, 100);

            // Configuração do eixo X
            Area.AxisX.Interval = 1;
            Area.AxisX.IntervalType = DateTimeIntervalType.Minutes;
            Area.AxisX.MajorGrid.LineColor = System.Drawing.Color.LightGray;
            Area.AxisX.LineWidth = 0;
            Area.AxisX.MajorGrid.Enabled = false; // Remove linhas principais
            Area.AxisX.MinorGrid.Enabled = false; // Remove linhas secundárias
            //Area.AxisX2.LineWidth               = 0;
            Area.AxisX.LabelStyle.Format = "F2";
            Area.AxisX.LabelStyle.ForeColor = Color.FromArgb(160, 160, 160);

            Area.AxisX.IsMarginVisible = false;

            // Alinha os valores do eixo Y à esquerda
            Area.AxisY.Enabled = AxisEnabled.False;
            //Area.AxisY.Maximum = 50;
            //Area.AxisY.Minimum = -50;
            Area.AxisY.LabelStyle.Format = "F2";
            Area.AxisY.LineWidth = 0;

            // Alinha os valores do eixo Y à direita
            Area.AxisY2.Enabled = AxisEnabled.True; // Habilita o segundo eixo Y
            Area.AxisY2.LabelStyle.Enabled = true; // Exibe os rótulos no eixo Y2
            Area.AxisY2.LabelStyle.Format = "F2"; // Formato dos rótulos
            Area.AxisY2.LineWidth = 0; // Remove a linha do eixo Y2, se desejado
            Area.AxisY2.MajorGrid.LineColor = Color.FromArgb(120, 120, 120);
            Area.AxisY2.MajorGrid.LineDashStyle = ChartDashStyle.Solid;
            Area.AxisY2.LabelStyle.ForeColor = Color.FromArgb(160, 160, 160);
            Area.AxisY2.LabelStyle.Font = new Font("Segoe UI", 8);
            Area.AxisY2.MajorGrid.Enabled = true; // Remove as linhas de grade do eixo Y2
            Area.AxisY2.MajorTickMark.Enabled = false; // Habilita as marcas de tique
            Area.AxisY2.IsLogarithmic = false;
            Area.AxisY2.IsStartedFromZero = false;
            Area.AxisY2.IsMarginVisible = false;

            Area.AxisY2.Maximum = 1;
            Area.AxisY2.Minimum = -1;

            //AddSerie("Candles", SeriesChartType.Candlestick);

            InitAxisX();
        }


        public void Suspend()
        {
            Chart.SuspendLayout();
        }
        public void Resume()
        {
            Chart.ResumeLayout();
        }


        public event Action<CandlePoint> SelectedCandle;
        private double? Step;
        private ChartArea Area;
        private double SizeX;
        private double OffsetX;
        private Chart Chart;
        private double LastMouseY;
        private double LastMouseX;
        private double StartMouseX;
        private double StartMouseY;
        private bool IsDragging = false;


        public ValueChart()
        {
            Init();
        }
    }

    public class CandlePoint : DataPoint
    {
        public double InputX;
        public Series Serie;
        public object Data;
    }


    public class DataPointDistance
    {
        public DataPoint Point { get; set; }
        public double Distance { get; set; }
        public int SerieIndex { get; set; }
    }

}
