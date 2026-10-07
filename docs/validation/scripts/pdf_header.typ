#show figure: set block(breakable: true)
#show table: set block(breakable: true)
#show table.cell.where(y: 0): set text(weight: "bold")
#set table(stroke: (x, y) => if y == 0 { (bottom: 0.6pt) } else { none }, inset: (x: 5pt, y: 3.5pt))
