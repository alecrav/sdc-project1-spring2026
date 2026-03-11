
find . -type f \( -name "*.cpp" -o -name "*.h" \) -print0 | \
xargs -0 sed -i '/\/\/ \[START Student Assignment\]/,/\/\/ \[END Student Assignment\]/c\
  // [START Student Assignment]\
  // Your turn to complete! \
  // [END Student Assignment]'
