

using System.Text.Json.Serialization;

namespace WinFormsApp1
{
    public class AdrressInfo
    {
        public int Address { get; set; }
        public string Description { get; set; }


        [JsonIgnore]
        public Label Label;

        [JsonIgnore]
        public Color ForeColor;

    }
}
