namespace WinFormsApp1
{
    partial class TitleEdit
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            Content = new RichTextBox();
            label1 = new Label();
            SuspendLayout();
            // 
            // Content
            // 
            Content.Anchor = AnchorStyles.Top | AnchorStyles.Bottom | AnchorStyles.Left | AnchorStyles.Right;
            Content.BorderStyle = BorderStyle.None;
            Content.Location = new Point(12, 55);
            Content.Name = "Content";
            Content.Size = new Size(257, 149);
            Content.TabIndex = 0;
            Content.Text = "";
            // 
            // label1
            // 
            label1.Anchor = AnchorStyles.Top | AnchorStyles.Left | AnchorStyles.Right;
            label1.ForeColor = SystemColors.HotTrack;
            label1.Location = new Point(12, 9);
            label1.Name = "label1";
            label1.Size = new Size(257, 39);
            label1.TabIndex = 1;
            label1.Text = "Cada linha representa descrição de um endereço. Exemplo: 234=Sample 01";
            // 
            // TitleEdit
            // 
            AutoScaleDimensions = new SizeF(7F, 15F);
            AutoScaleMode = AutoScaleMode.Font;
            ClientSize = new Size(281, 216);
            Controls.Add(label1);
            Controls.Add(Content);
            Name = "TitleEdit";
            Text = "TitleEdit";
            ResumeLayout(false);
        }

        #endregion
        private Label label1;
        public RichTextBox Content;
    }
}