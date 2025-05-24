namespace CapturaTestesGUI.Utils
{

    public class ColorGenerator
    {

        public static Color Create()
        {
            Color result = Color.Black;

            int ix = 0;
            while (ix < MAX_ATTEMPTS)
            {
                result = Color.FromArgb(Rand.Next(MIN_COLOR, MAX_COLOR), Rand.Next(MIN_COLOR, MAX_COLOR), Rand.Next(MIN_COLOR, MAX_COLOR));

                int ij = 0;
                while (ij < LastColors.Count)
                {
                    var dr = Math.Abs(result.R - LastColors[ij].R);
                    var dg = Math.Abs(result.G - LastColors[ij].G);
                    var db = Math.Abs(result.B - LastColors[ij].B);

                    if (dr < MIN_DIF || dg < MIN_DIF || db < MIN_DIF)
                    {
                        break;
                    }
                    ij++;
                }

                if (ij >= LastColors.Count)
                {
                    LastColors.Add(result);
                    break;
                }

                ix++;
            }

            if (LastColors.Count > MAX_CNT)
            {
                LastColors.RemoveAt(0);
            }

            return result;
        }


        private static int MIN_COLOR    = 170;
        private static int MAX_COLOR    = 245;
        private static int MIN_DIF      = 26;
        private static int MAX_ATTEMPTS = 100;
        private static int MAX_CNT      = 6;

        private static Random Rand;
        private static List<Color> LastColors;


        static ColorGenerator()
        {
            Rand = new Random((int)DateTime.Now.Ticks);
            LastColors = new List<Color>();




            LastColors.Add(Color.Black);
        }

    }

}
