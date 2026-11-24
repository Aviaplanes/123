using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Windows.Forms;

namespace MemoryGame
{
    // Игра «Найди пару»: поле 4x4 (16 клеток = 8 пар эмодзи).
    // Кликни по клетке, открой две — если эмодзи совпали, пара остаётся,
    // иначе клетки снова закроются. Найди все 8 пар, чтобы выиграть.
    public partial class Form1 : Form
    {
        private readonly string[] _emojis = { "🍎", "🍌", "🍓", "🍇", "🍉", "🍒", "🥝", "🍑" };
        private readonly Random _rnd = new();
        private Button _first = null;   // первая открытая клетка
        private int _matched = 0;       // сколько пар найдено

        public Form1()
        {
            Text = "Найди пару 🎯";
            ClientSize = new Size(440, 480);

            // Делаем колоду: по 2 одинаковых эмодзи на каждую пару, затем перемешиваем.
            var deck = _emojis.Concat(_emojis).OrderBy(_ => _rnd.Next()).ToList();

            for (int i = 0; i < 16; i++)
            {
                var b = new Button
                {
                    Text = "❓",
                    Font = new Font("Segoe UI Emoji", 26),
                    Size = new Size(100, 100),
                    Location = new Point(10 + (i % 4) * 105, 10 + (i / 4) * 105),
                    Tag = deck[i]   // правильный эмодзи хранится в Tag
                };
                b.Click += OnClick;
                Controls.Add(b);
            }
        }

        private void OnClick(object sender, EventArgs e)
        {
            var b = (Button)sender;
            if (_first == null)                       // открываем первую клетку
            {
                _first = b;
                b.Text = (string)b.Tag;
                return;
            }
            if (_first == b) return;                  // повторный клик по той же — игнор

            b.Text = (string)b.Tag;                   // открываем вторую клетку

            if ((string)_first.Tag == (string)b.Tag)  // совпала?
            {
                _first.Enabled = b.Enabled = false;   // пара найдена — оставляем
                _first = null;
                if (++_matched == 8) MessageBox.Show("Победа! 🎉");
            }
            else                                      // не совпала — прячем обе
            {
                var first = _first;
                _first = null;
                var timer = new Timer { Interval = 600 };
                timer.Tick += (_, _) => { first.Text = b.Text = "❓"; timer.Dispose(); };
                timer.Start();
            }
        }
    }
}
