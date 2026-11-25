using System.Drawing;
using System.Windows.Forms;

static class Program
{
    [STAThread]
    static void Main()
    {
        ApplicationConfiguration.Initialize();
        Application.Run(new MemoryGame());
    }
}

sealed class MemoryGame : Form
{
    readonly string[] emoji = ["🍎", "🍌", "🍓", "🍇", "🍉", "🍒", "🥝", "🍑"];
    readonly Button[] cards = new Button[16];
    readonly Label score = new() { Dock = DockStyle.Top, Height = 64,
        TextAlign = ContentAlignment.MiddleCenter, Font = new Font("Segoe UI", 14) };
    readonly System.Windows.Forms.Timer timer = new() { Interval = 750 };
    Button? first, second;
    int moves, pairs;

    public MemoryGame()
    {
        Text = "Найди пару • Эмодзи";
        ClientSize = new Size(440, 550);
        MinimumSize = new Size(360, 480);
        StartPosition = FormStartPosition.CenterScreen;
        BackColor = Color.FromArgb(242, 244, 250);
        var grid = new TableLayoutPanel { Dock = DockStyle.Fill,
            ColumnCount = 4, RowCount = 4, Padding = new Padding(8) };
        for (int i = 0; i < 4; i++)
        {
            grid.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 25));
            grid.RowStyles.Add(new RowStyle(SizeType.Percent, 25));
        }
        for (int i = 0; i < cards.Length; i++)
        {
            var card = new Button { Dock = DockStyle.Fill, Margin = new Padding(4),
                Font = new Font("Segoe UI Emoji", 28), UseVisualStyleBackColor = false };
            card.Click += (_, _) => Open(card);
            grid.Controls.Add(card, i % 4, i / 4);
            cards[i] = card;
        }
        var restart = new Button { Text = "Новая игра", Dock = DockStyle.Bottom,
            Height = 54, Font = new Font("Segoe UI", 12) };
        restart.Click += (_, _) => NewGame();
        timer.Tick += (_, _) =>
        {
            timer.Stop();
            HideCard(first!);
            HideCard(second!);
            first = second = null;
        };
        FormClosed += (_, _) => timer.Dispose();
        Controls.Add(grid);
        Controls.Add(score);
        Controls.Add(restart);
        NewGame();
    }

    void NewGame()
    {
        timer.Stop();
        first = second = null;
        moves = pairs = 0;
        var deck = emoji.Concat(emoji).ToArray();
        Random.Shared.Shuffle(deck);
        for (int i = 0; i < cards.Length; i++)
        {
            cards[i].Tag = deck[i];
            cards[i].Enabled = true;
            HideCard(cards[i]);
        }
        UpdateScore();
    }

    static void HideCard(Button card)
    {
        card.Text = "❓";
        card.BackColor = Color.White;
    }

    void Open(Button card)
    {
        if (second != null || card == first || !card.Enabled) return;
        card.Text = (string)card.Tag!;
        card.BackColor = Color.LightYellow;
        if (first == null) { first = card; return; }
        moves++;
        if (Equals(first.Tag, card.Tag))
        {
            first.Enabled = card.Enabled = false;
            first.BackColor = card.BackColor = Color.Honeydew;
            first = null;
            pairs++;
        }
        else { second = card; timer.Start(); }
        UpdateScore();
    }

    void UpdateScore() => score.Text = pairs == 8
        ? $"🎉 Победа! Ходов: {moves}"
        : $"Пары: {pairs}/8     •     Ходы: {moves}";
}
