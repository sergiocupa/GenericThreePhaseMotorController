

namespace WinFormsApp1
{
    public class SerieChart
    {
        public int Index;
        public string Name;
        public SerieLineType LineType;
        public float Thickness;
        public Color? Color;
        public List<PointChart> Points;

        public double RangeX;
        public double RangeY;
        public double ValueMinX;
        public double ValueMaxX;
        public double ValueMinY;
        public double ValueMaxY;

        public double StepX;
        public double StepY;


        public SerieChart()
        {
            LineType = SerieLineType.None;
            Points   = new List<PointChart>();
        }
    }

    public class PointChart
    {
        public ulong Index;
        public double X;
        public double Y;
    }

    public enum SerieLineType
    {
        None  = 0,
        Line  = 1,
        Dotted = 2
    }
}
