from collections import defaultdict


class ExpenseSplitter:

  def __init__(self):
    self.members = set()
    self.balances = defaultdict(float)
    self.total_spent = defaultdict(float)

  def add_member(self, name: str):
    name = name.strip()
    if name:
      self.members.add(name)

  def add_expense(
      self, payer: str, amount: float, participants: list[str]
  ) -> bool:
    if not participants or amount <= 0:
      return False

    split_amount = amount / len(participants)
    self.total_spent[payer] += amount
    self.balances[payer] += amount

    for person in participants:
      self.balances[person] -= split_amount
    return True

  def simplify_debts(self):
    debtors = []
    creditors = []

    for person in self.members:
      net = round(self.balances[person], 2)
      if net < -0.01:
        debtors.append([person, -net])
      elif net > 0.01:
        creditors.append([person, net])

    debtors.sort(key=lambda x: x[1], reverse=True)
    creditors.sort(key=lambda x: x[1], reverse=True)

    transactions = []
    i, j = 0, 0

    while i < len(debtors) and j < len(creditors):
      settle_amt = min(debtors[i][1], creditors[j][1])
      transactions.append((debtors[i][0], creditors[j][0], settle_amt))

      debtors[i][1] -= settle_amt
      creditors[j][1] -= settle_amt

      if abs(debtors[i][1]) < 0.01:
        i += 1
      if abs(creditors[j][1]) < 0.01:
        j += 1

    return transactions

  def display_report(self):
    if not self.members:
      print("\n[!] Chua co thanh vien nao trong he thong.")
      return

    print("\n" + "=" * 50)
    print("               BAO CAO CHI TIEU")
    print("=" * 50)
    print(f"{'Thanh vien':<15} | {'Da chi':<15} | {'So du rong':<15}")
    print("-" * 50)
    for person in sorted(self.members):
      spent = self.total_spent[person]
      net = self.balances[person]
      status = f"+{net:,.0f}" if net > 0 else f"{net:,.0f}"
      print(f"{person:<15} | {spent:>12,.0f} d | {status:>12} d")

    print("\n" + "=" * 50)
    print("         LICH CHUYEN KHOAN TOI UU NHAT")
    print("=" * 50)
    txns = self.simplify_debts()
    if not txns:
      print("Tat ca da can bang, khong co ai no ai.")
    else:
      for debtor, creditor, amt in txns:
        print(f"👉 {debtor:<10} chuyen cho {creditor:<10} : {amt:>10,.0f} VND")
    print("=" * 50 + "\n")


def main():
  app = ExpenseSplitter()

  print("=== HE THONG QUAN LY CHI TIEU PHONG / NHOM ===")
  raw_members = input(
      "Nhap danh sach ten thanh vien (cach nhau boi dau phay): "
  )
  for name in raw_members.split(","):
    app.add_member(name.strip())

  if not app.members:
    print("Danh sach trong. Dung chuong trinh.")
    return

  print(f"\nDa khoi tao {len(app.members)} thanh vien: {', '.join(app.members)}")

  while True:
    print("\n--- MENU ---")
    print("1. Them khoan chi tieu moi")
    print("2. Xem bao cao tong ket va chuyen khoan")
    print("3. Thoat chuong trinh")
    choice = input("Chon thao tac (1/2/3): ").strip()

    if choice == "1":
      payer = input(
          f"Nguoi tra tien ({'/'.join(app.members)}): "
      ).strip()
      if payer not in app.members:
        print("[!] Nguoi nay khong co trong danh sach phong.")
        continue

      try:
        amount = float(input("So tien da tra (VND): ").strip())
        if amount <= 0:
          print("[!] So tien phai lon hon 0.")
          continue
      except ValueError:
        print("[!] So tien khong hop le.")
        continue

      print("Chon cach chia:")
      print("  a. Chia deu tat ca moi nguoi trong phong")
      print("  b. Chia cho danh sach cu the")
      split_mode = input("Lua chon (a/b): ").strip().lower()

      if split_mode == "a":
        participants = list(app.members)
      elif split_mode == "b":
        raw_part = input("Nhap danh sach nguoi chia (cach boi dau phay): ")
        participants = [p.strip() for p in raw_part.split(",") if p.strip()]
        # Kiem tra hop le
        invalid = [p for p in participants if p not in app.members]
        if invalid:
          print(f"[!] Co nguoi khong ton tai: {', '.join(invalid)}")
          continue
      else:
        print("[!] Lua chon khong hop le.")
        continue

      if app.add_expense(payer, amount, participants):
        print(
            f"[OK] Da ghi nhan khoan chi: {payer} tra {amount:,.0f} VND chia"
            f" cho {len(participants)} nguoi."
        )

    elif choice == "2":
      app.display_report()

    elif choice == "3":
      print("Ket thuc chuong trinh.")
      break
    else:
      print("[!] Lua chon khong hop le, vui long chon lai.")


if __name__ == "__main__":
  main()